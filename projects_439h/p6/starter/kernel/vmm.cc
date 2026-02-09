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


namespace VMM {

void global_init() {
    MISSING();
}

void per_core_init() {
    MISSING();
}

void naive_munmap(void* p_) {
    MISSING();
}

void* naive_mmap(uint32_t sz_, bool shared, StrongPtr<Node> node, uint32_t offset_) {
    MISSING();
    return nullptr;
}

} /* namespace vmm */

extern "C" void vmm_pageFault(uintptr_t va_, uintptr_t *saveState) {
    Debug::panic("*** can't handle page fault at %x\n",va_);
}
