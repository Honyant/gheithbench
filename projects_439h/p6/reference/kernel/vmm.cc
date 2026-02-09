#include "vmm.h"
#include "machine.h"
#include "idt.h"
#include "libk.h"
#include "blocking_lock.h"
#include "config.h"
#include "threads.h"
#include "debug.h"
#include "ext2.h"
#include "physmem.h"
#include <cstdint>
#include <cstdio>

BlockingLock vmm_lock{};

namespace VMM {

uint32_t global_pd;
VME* shared_VMEs = nullptr;
const uint32_t SHARED_START = 0xF0000000;
const uint32_t SHARED_END = 0xFFFFFFFF;
const uint32_t IOAPIC_PAGE = 0xFEC00000;
const uint32_t LAPIC_PAGE = 0xFEE00000;
BlockingLock shared_lock{};


void pd_map_unpresent(uint32_t* pd, uint32_t va, uint32_t pa);

void* memset(void *ptr, int value, size_t num) {
    uint8_t* p = (uint8_t*)ptr;
    for (size_t i = 0; i < num; i++) {
        p[i] = (uint8_t)value;
    }
    return ptr;
}

void global_init() {
    global_pd = PhysMem::alloc_frame();
    for(uint32_t i = PhysMem::FRAME_SIZE; i < kConfig.memSize; i += PhysMem::FRAME_SIZE){
        VMM::pd_map((uint32_t*) global_pd, i, i);
    }
    for(uint32_t i = 0xF0000000; i < 0xFFFFF000; i += PhysMem::FRAME_SIZE){
        if(i == kConfig.ioAPIC || i == kConfig.localAPIC) continue;
        pd_map_unpresent((uint32_t*) global_pd, i, i);
    }
    VMM::pd_map((uint32_t*) global_pd, kConfig.ioAPIC, kConfig.ioAPIC);
    VMM::pd_map((uint32_t*) global_pd, kConfig.localAPIC, kConfig.localAPIC);
}

void per_core_init() {
    vmm_on(global_pd);
}

FreeRegion* shared_free_regions = nullptr;

void init_shared_regions() {
    // region before IOAPIC
    FreeRegion* r1 = new FreeRegion();
    r1->start = SHARED_START;
    r1->size = IOAPIC_PAGE - SHARED_START;
    r1->next = nullptr;
    r1->prev = nullptr;
    
    // region between IOAPIC and LAPIC
    FreeRegion* r2 = new FreeRegion();
    r2->start = IOAPIC_PAGE + PhysMem::FRAME_SIZE;
    r2->size = LAPIC_PAGE - (IOAPIC_PAGE + PhysMem::FRAME_SIZE);
    r2->next = nullptr;
    r2->prev = r1;
    
    // region after LAPIC
    FreeRegion* r3 = new FreeRegion();
    r3->start = LAPIC_PAGE + PhysMem::FRAME_SIZE;
    r3->size = SHARED_END - (LAPIC_PAGE);
    r3->next = nullptr;
    r3->prev = r2;
    
    r1->next = r2;
    r2->next = r3;
    
    shared_free_regions = r1;
}

void init_free_regions() {
    FreeRegion* fr = new FreeRegion();
    fr->start = 0x80000000;
    fr->size = 0x70000000;
    fr->next = nullptr;
    fr->prev = nullptr;
    impl::threads::state.current()->free_regions = fr;
}
void* naive_mmap(uint32_t sz_, bool shared, StrongPtr<Node> node, uint32_t offset_) {
    if (sz_ == 0) return nullptr;
    uint32_t sz = (sz_ + 0xFFF) & ~0xFFF;
    uint32_t va = 0;
    FreeRegion* prev_fr = nullptr;
    FreeRegion* fr = nullptr;
    
    if (shared) {
        shared_lock.lock();
        if (!shared_free_regions) init_shared_regions();
        fr = shared_free_regions;
    } else {
        if (!impl::threads::state.current()->has_init_free_regions) {
            init_free_regions();
            impl::threads::state.current()->has_init_free_regions = true;
        }
        fr = impl::threads::state.current()->free_regions;
    }

    // find suitable region
    while (fr && fr->size < sz) {
        prev_fr = fr;
        fr = fr->next;
    }
    
    if (!fr) {
        if (shared) shared_lock.unlock();
        return nullptr;
    }

    va = fr->start;
    if (fr->size == sz) {
        // remove entire region
        (prev_fr ? prev_fr->next : (shared ? shared_free_regions : 
            impl::threads::state.current()->free_regions)) = fr->next;
        if (fr->next) fr->next->prev = prev_fr;
        delete fr;
    } else {
        // shrink region
        fr->start += sz;
        fr->size -= sz;
    }

    // create and insert VME
    VME* m = new VME{va, sz, shared, node, offset_, 
        shared ? shared_VMEs : impl::threads::state.current()->VMEs};
    if (shared) {
        shared_VMEs = m;
        shared_lock.unlock();
    } else {
        impl::threads::state.current()->VMEs = m;
    }

    return (void*)va;
}

void naive_munmap(void* p_) {
    if (p_ >= (void*)0xF0000000) return;
    uint32_t addr = (uint32_t)p_;
    
    // find VME
    VME* prev = nullptr;
    VME* m = impl::threads::state.current()->VMEs;
    while (m && !(m->start <= addr && m->start + m->size > addr)) {
        prev = m;
        m = m->next;
    }
    
    if (!m || m->shared) return;

    // remove VME from list
    (prev ? prev->next : impl::threads::state.current()->VMEs) = m->next;

    // unmap pages
    uint32_t cr3;
    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    for (uint32_t addr = m->start; addr < m->start + m->size; addr += PhysMem::FRAME_SIZE) {
        VMM::pd_unmap((uint32_t*)cr3, addr);
        asm volatile("invlpg (%0)" :: "r"(addr) : "memory");
    }

    // create and insert free region
    FreeRegion* fr = new FreeRegion{(uint32_t)m->start, m->size, nullptr, nullptr};
    FreeRegion *curr = impl::threads::state.current()->free_regions, *prev_fr = nullptr;
    
    while (curr && curr->start < fr->start) {
        prev_fr = curr;
        curr = curr->next;
    }

    fr->next = curr;
    fr->prev = prev_fr;
    if (curr) curr->prev = fr;
    (prev_fr ? prev_fr->next : impl::threads::state.current()->free_regions) = fr;

    // coalesce regions
    while (fr->prev && fr->prev->start + fr->prev->size == fr->start) {
        FreeRegion* p = fr->prev;
        p->size += fr->size;
        p->next = fr->next;
        if (fr->next) fr->next->prev = p;
        delete fr;
        fr = p;
    }
    while (fr->next && fr->start + fr->size == fr->next->start) {
        FreeRegion* n = fr->next;
        fr->size += n->size;
        fr->next = n->next;
        if (n->next) n->next->prev = fr;
        delete n;
    }

    delete m;
}

void pd_map(uint32_t* pd, uint32_t va, uint32_t pa){
    uint32_t pd_idx = va >> 22;
    uint32_t pt_idx = (va >> 12) & 0x3FF;
    uint32_t pde = pd[pd_idx];
    if(!(pde & 0x1)){ // if not present, create a new page table
        pde = PhysMem::alloc_frame() | 0x3; // present, read/write
        pd[pd_idx] = pde; // set the page directory entry
    }
    uint32_t* pt = (uint32_t*)(pde & 0xFFFFF000);
    pt[pt_idx] = pa | 0x3;
}

void pd_map_unpresent(uint32_t* pd, uint32_t va, uint32_t pa){
    uint32_t pd_idx = va >> 22;
    uint32_t pt_idx = (va >> 12) & 0x3FF;
    uint32_t pde = pd[pd_idx];
    if(!(pde & 0x1)){ // if not present, create a new page table
        pde = PhysMem::alloc_frame() | 0x3; // present, read/write
        pd[pd_idx] = pde; // set the page directory entry
    }
    uint32_t* pt = (uint32_t*)(pde & 0xFFFFF000);
    pt[pt_idx] = pa | 0x2;
}

void pd_unmap(uint32_t* pd, uint32_t va){
    uint32_t pd_idx = va >> 22;
    uint32_t pt_idx = (va >> 12) & 0x3FF;
    uint32_t pde = pd[pd_idx];
    // Debug::ASSERT(pde & 0x1);
    if(pde & 0x1){
        uint32_t* pt = (uint32_t*)(pde & 0xFFFFF000);
        // Debug::printf("Deallocating frame %x\n", pt[pt_idx] & 0xFFFFF000);
        if (pt[pt_idx] & 0x1)
            PhysMem::dealloc_frame(pt[pt_idx] & 0xFFFFF000);
        pt[pt_idx] = 0;
    }
}

} /* namespace vmm */

extern "C" void vmm_pageFault(uintptr_t va_, uintptr_t *saveState) {
    if(va_ < 0x1000){
        Debug::panic("null pointer dereference: %d\n", va_);
    }
    
    va_ &= ~0xFFF;
    VMM::VME* m = va_ >= 0xF0000000 ? VMM::shared_VMEs : impl::threads::state.current()->VMEs;
    while (m != nullptr) {
        if (va_ >= m->start && (m->start + m->size < m->start? va_ < 0xffffffff: va_ < m->start + m->size)) {
            break;
        }
        m = m->next;
    }
    
    if (m == nullptr) {
        Debug::panic("Unmapped page fault at %x\n", va_);
    }
    if (va_ >= 0xF0000000) {
        VMM::shared_lock.lock();
        // check if the page is already mapped
        uint32_t cr3;
        asm volatile("mov %%cr3, %0" : "=r"(cr3));
        uint32_t* pd = (uint32_t*)cr3;
        uint32_t pd_idx = va_ >> 22;
        uint32_t pt_idx = (va_ >> 12) & 0x3FF;
        uint32_t pde = pd[pd_idx];
        // if the page is already mapped, return
        if(pde & 0x1){
            uint32_t* pt = (uint32_t*)(pde & 0xFFFFF000);
            if(pt[pt_idx] & 0x1){
                VMM::shared_lock.unlock();
                return;
            }
        }
    }

    uint32_t pa = PhysMem::alloc_frame();
    if (!(m->node == nullptr)) {
        m->node->read_all(va_ - m->start + m->offset, PhysMem::FRAME_SIZE, (char*) pa);
    }
    uint32_t cr3;
    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    uint32_t* pd = (uint32_t*)cr3;
    VMM::pd_map(pd, va_ & ~0xFFF, pa);
    asm volatile("invlpg (%0)" : : "r" (va_) : "memory");
    if(va_ >= 0xF0000000){
        VMM::shared_lock.unlock();
    }
}
