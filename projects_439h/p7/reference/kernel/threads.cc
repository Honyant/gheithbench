#include "debug.h"
#include "smp.h"
#include "debug.h"
#include "config.h"
#include "vmm.h"
#include "physmem.h"
#include "pit.h"
#include "threads.h"
#include "atomic.h"
#include "blocking_queue.h"
#include "tss.h"
#include "shared.h"
#include "ext2.h"
#include <cstdint>

namespace impl::threads {


    TCB::~TCB() {
        // Clean up all VMEs and their associated pages
        // Debug::printf("cleaning up VMEs\n");
        VMM::VME* curr = VMEs;
        while (curr != nullptr) {
                // Unmap and deallocate all pages in this VME's range
            // Debug::printf("address: %x, size: %x\n", curr->start, curr->size);
            for (uint32_t addr = curr->start; 
                    addr < curr->start + curr->size; 
                    addr += PhysMem::FRAME_SIZE) {
                VMM::pd_unmap((uint32_t*) current_pd, addr);
            }
            // Move to next VME
            VMM::VME* next = curr->next;
            // delete curr;
            curr = next;
        }

        // Clean up free regions list
        VMM::FreeRegion* fr = free_regions;
        while (fr != nullptr) {
            VMM::FreeRegion* next = fr->next;
            // delete fr;
            fr = next;
        }

        if (current_pd != 0)
            PhysMem::dealloc_frame(current_pd);
    }

    // State constructor
    State::State() {

        reaper_queue = new BlockingQueue<TCB>();

        // The reaper thread, needs to run in a thread in order to
        // be able to access the heap and contend for the queue
        thread([this] {
            while(true) {

                auto p = reaper_queue->remove();
                ASSERT(p != nullptr);
                
                delete p;
            }
        });
    }

    State state{};

    void reap() {
        auto p = state.reaper_queue->remove_all();
        while (p != nullptr) {
            auto next = p->next;
            delete p;
            p = next;
        }
    }

    void SleepQueue::add(TCB* tcb, uint32_t after_seconds) {
        ASSERT(tcb != nullptr);                    // must have a TCB
        ASSERT(state.in_helper_thread());    // can only run with preemption disabled

        TCB** pprev = &first;
        TCB* p = first;

        tcb->at_jiffies = Pit::jiffies + Pit::secondsToJiffies(after_seconds);

        while ((p != nullptr) && (p->at_jiffies <= tcb->at_jiffies)) {
            pprev = &p->next;
            p = p->next;
        }

        tcb->next = *pprev;
        *pprev = tcb;
    }

    TCB* SleepQueue::remove() {
        ASSERT(state.in_helper_thread()); // can only run in the helper thread with preemption disabled
        auto p = first;

        if ((p == nullptr) || (p->at_jiffies > Pit::jiffies)) {
            return nullptr;
        } else {
            first = p->next;
            p->next = nullptr;
            return p;
        }
    }

    void* memset(void *ptr, int value, size_t num) {
        uint8_t* p = (uint8_t*)ptr;
        for (size_t i = 0; i < num; i++) {
            p[i] = (uint8_t)value;
        }
        return ptr;
    }

    [[noreturn]]
    void thread_entry() {
        ASSERT(!Interrupts::isDisabled());
        auto me = state.current();
        uint32_t current_pd = PhysMem::alloc_frame();
        for(uint32_t i = 0; i < kConfig.memSize; i += PhysMem::FRAME_SIZE * 1024){
            ((uint32_t*)current_pd)[i>>22] = ((uint32_t*)VMM::global_pd)[i>>22];
        }
        for(uint32_t i = 0xF0000000; i < 0xFFC00000; i += PhysMem::FRAME_SIZE * 1024){
            ((uint32_t*)current_pd)[i>>22] = ((uint32_t*)VMM::global_pd)[i>>22];
        }

        if (me->prev_pd != 0) { // if the thread is a forked thread       
            for(uint32_t i = 512; i < 1024 - 64; i++) { // only copy the user space mappings
                if (((uint32_t*)me->prev_pd)[i] & 0x1) {
                    uint32_t prev_pde = ((uint32_t*)me->prev_pd)[i];
                    uint32_t prev_pt = prev_pde & 0xFFFFF000;

                    // loop through the page table
                    bool need_new_page_table = false;
                    for(uint32_t j = 0; j < 1024; j++) {
                        uint32_t prev_pte = ((uint32_t*)prev_pt)[j];
                        if(prev_pte) {
                            need_new_page_table = true;
                            break;
                        }
                    }
                    if (!need_new_page_table) {
                        continue;
                    }
                    // always need to allocate the page table to maintain the mappings
                    ((uint32_t*)current_pd)[i] = PhysMem::alloc_frame() | (prev_pde & 0xFFF);
                    uint32_t current_pt = ((uint32_t*)current_pd)[i] & 0xFFFFF000;

                    for(uint32_t j = 0; j < 1024; j++) {
                        uint32_t prev_pte = ((uint32_t*)prev_pt)[j];
                        if(prev_pte) {
                            // Debug::printf("VA: %x\n", (i << 22) | (j << 12));
                            ((uint32_t*)current_pt)[j] = PhysMem::alloc_frame() | (prev_pte & 0xFFF);
                            uint32_t current_page = ((uint32_t*)current_pt)[j] & 0xFFFFF000;
                            uint32_t prev_page = prev_pte & 0xFFFFF000;
                            memcpy((void*)current_page, (void*)prev_page, PhysMem::FRAME_SIZE);
                        }
                    }
                }
            }
            // Debug::printf("Done copying pages in forked thread\n");
            vmm_on(current_pd);
            me->current_pd = current_pd;
            me->prev_pd = 0;
            // Debug::printf("Turned on new page directory for forked thread\n");
        }
        else{
            vmm_on(current_pd);
            me->current_pd = current_pd;
        }
        ASSERT(me != nullptr);
        // Debug::printf("thread_entry: %x\n", me);
        me->doit();
        stop();
    }
    

    [[noreturn]]
    void helper() {
        auto id = SMP::me();
        ASSERT(state.in_helper_thread());

        auto wakeup = [id] {
            auto p = state.sleep_queues[id].remove();
            while (p != nullptr) {
                state.ready_queue.add(p);
                p = state.sleep_queues[id].remove();
            }
        };

        while (true) {
            ASSERT(state.in_helper_thread());      // nullptr -> helper thread
            ASSERT(!Interrupts::isDisabled());         // interrupts should be enabled
            ASSERT(SMP::me() == id);                   // has affinity to a core


            wakeup();

            auto request = state.help_requests[id];
            if (request != nullptr) {
                state.help_requests[id] = nullptr;
                request->doit();
            }

            TCB* next = state.ready_queue.remove();
            while (next == nullptr) {
                iAmStuckInALoop(false);
                wakeup();
                next = state.ready_queue.remove();
            }

            // setting active_thread enables preemption, need to disable interrupts briefly
            cli();     
            state.active_thread[id] = next;                   // O(1)
            tss[id].esp0 = next->interruptEsp();                    // O(1)
            context_switch(&state.helpers[id], &next->save_area);   // O(1)
            sti();   
        }
    }
}

using namespace impl::threads;

void yield() {
    // Performance optimization. In the worse case, the data race will
    // cause us to miss a ready thread once but will find it when we
    // check again.
    reap();

    if (state.ready_queue.isEmpty()) return;


    auto tcb = state.current();
    ASSERT(tcb != nullptr);
    state.block("yield", [tcb] {
        // run in the helper thread
        state.ready_queue.add(tcb);
    });
}

void sleep(uint32_t sec) {
    reap();
    auto tcb = state.current();
    ASSERT(tcb != nullptr);
    state.block("sleep",[tcb, sec] {
        // run in helper thread with preemption disabled
        state.sleep_queues[SMP::me()].add(tcb, sec);
    });
}

[[noreturn]]
void stop() {
    auto tcb = state.current();

    if (tcb == nullptr) {
        // only the initial cores call stop with current thread set to nullptr, they
        // morph into the helper threads
        helper();
    } else {
        reap();

        // a created thread is attempting to stop
        state.block("stop", [tcb] {
            // run in helper thread, can't delete here because delete uses the heap
            // and that uses a blocking lock but the helper thread is not allowed to block
            state.reaper_queue->add(tcb);
        });
        PANIC("the impossible has happened");
    }
}


