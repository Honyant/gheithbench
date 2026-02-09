#pragma once

#include "atomic.h"
#include "queue.h"
#include "heap.h"
#include "debug.h"
#include "smp.h"

constexpr size_t STACK_BYTES = 8 * 0124;
constexpr size_t STACK_WORDS = STACK_BYTES / sizeof(uintptr_t);  /* sizeof(word) == size(void*) */

namespace impl::threads {

    // Limitations of current implementation:
    //    - assumes single core
    //    - no preemption

    struct TCB {
        TCB* next = nullptr;
        uintptr_t sp;
        TCB* came_from = nullptr;

        virtual void doit() = 0;
    };

    extern void thread_entry();

    template <typename Work>
    struct TCBWithWork: public TCB {
        uintptr_t stack[STACK_WORDS];
        Work work;

        TCBWithWork(Work const& work): work(work) {
            auto stack_index = STACK_WORDS-1;
            auto push = [&stack_index, this](uintptr_t v) {
                stack[stack_index --] = v;
            };
            push((uintptr_t) thread_entry);     // return addr
            push(0);     // ebp
            push(0);     // edi
            push(0);     // esi
            push(0);     // ebx

            sp = (uintptr_t) &stack[stack_index+1];
        }

        void doit() override {
            work();
        }

    };

    extern Queue<TCB, NoLock> ready_queue;
    extern TCB* current_thread;

    extern "C" void context_switch(uintptr_t* from, uint32_t to);

    template <typename LockType>
    void block(Queue<TCB, LockType> *q) {
        auto next = ready_queue.remove();
        if (next == nullptr) return;

        auto me = current_thread;
        current_thread = next; // inconsistent state, current_thread will eventually be correct
        q->add(me); // inconsistent state, running and queued
        context_switch(&me->sp, next->sp);
    }
};

extern void stop();
extern void yield();

template <typename T>
void thread(T const& f) {
    using namespace impl::threads;
    auto tcb = new TCBWithWork(f);
    ready_queue.add(tcb);
    
}

