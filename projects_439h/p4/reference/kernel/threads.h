#pragma once

#include "atomic.h"
#include "queue.h"
#include "heap.h"
#include "debug.h"
#include "smp.h"

constexpr size_t STACK_BYTES = 4 * 1024;
constexpr size_t STACK_WORDS = STACK_BYTES / sizeof(uintptr_t);  /* sizeof(word) == size(void*) */

namespace impl::threads {
    extern void thread_entry();
    extern void offline_interrupt_handler();

    struct TCB {
        TCB* next = nullptr;
        uintptr_t sp;
        TCB* came_from = nullptr;
        Queue<TCB, NoLock>* nl_queue = nullptr;
        Queue<TCB, SpinLock>* spin_queue = nullptr;
        bool inApit = false;
        bool busy = false;
        bool stop = false;
        SpinLock* lock = nullptr;
        int* dc_semaphore_count = nullptr;
        uint32_t wake_jiffies = 0;
        bool is_sleeping = false;
        bool is_helper = false;
        TCB* next_sleep = nullptr;
        void clearsem() {
            nl_queue = nullptr;
            lock = nullptr;
            dc_semaphore_count = nullptr;
        }
        virtual ~TCB(){
            
        }
        virtual void doit() = 0;
    };
    
    extern void reaper_function();

    struct ReaperThread : public TCB {
        uintptr_t stack[STACK_WORDS];

        ReaperThread() {
            busy = false;
            auto stack_index = STACK_WORDS-1;
            auto push = [&stack_index, this](uintptr_t v) {
                stack[stack_index --] = v;
            };
            push((uintptr_t) reaper_function);  // Start at reaper_function instead of thread_entry
            push(0);     // ebx
            
            push(0);     // esi
            push(0);     // edi
            push(0);     // ebp
            push(0x200);   // interrupts enabled when we start
            sp = (uintptr_t) &stack[stack_index+1];
        }

        ~ReaperThread() override = default;

        void doit() override {
            PANIC("ReaperThread::doit() should never be called");
        }
    };

    // Global ReaperThread pointer
    extern ReaperThread* reaper_thread;

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
            push(0);     // ebx
            push(0);     // esi
            push(0);     // edi
            push(0);     // ebp
            push(0x200);   // interrupts enabled when we start

            sp = (uintptr_t) &stack[stack_index+1];
        }
        ~TCBWithWork(){
            // delete &stack;
        }

        void doit() override {
            work();
        }

    };

    struct Helper: public TCB {
        uintptr_t stack[STACK_WORDS];
        Helper() {
            is_helper = true;
            busy = true;
            auto stack_index = STACK_WORDS-1;
            auto push = [&stack_index, this](uintptr_t v) {
                stack[stack_index --] = v;
            };
            push((uintptr_t) offline_interrupt_handler);     // return addr
            push(0);     // ebx
            push(0);     // esi
            push(0);     // edi
            push(0);     // ebp
            push(0x200);   // interrupts enabled when we start
            sp = (uintptr_t) &stack[stack_index+1];
        }
        ~Helper(){
            // delete &stack;
        }

        void doit() override {
            
        }

    };

    extern TCB** helper_threads;
    extern TCB** current_threads;
    extern Queue<TCB, SpinLock> ready_queue;
    extern TCB* me_thread();
    
    template <typename LockType>
    extern void block(Queue<TCB,LockType>* q);
    extern bool blocknl(Queue<TCB, NoLock>* q, int* counter, SpinLock* s);

    extern void post_switch();
    extern "C" void context_switch(uintptr_t* from, uintptr_t* to);


};

[[noreturn]]
extern void stop();
extern void yield();
extern void sleep(uint32_t seconds);

template <typename T>
void thread(T const& f) {
    using namespace impl::threads;
    auto tcb = new TCBWithWork(f);
    ready_queue.add(tcb);
}