#pragma once

#include "atomic.h"
#include "queue.h"
#include "heap.h"
#include "debug.h"
#include "smp.h"
#define test sizeof(T) == 1

constexpr size_t STACK_BYTES = 8 * 1024;
constexpr size_t STACK_WORDS = STACK_BYTES / sizeof(uintptr_t);  /* sizeof(word) == size(void*) */

namespace impl::threads {

    // Limitations of current implementation:
    //    - assumes single core
    //    - no preemption
    
    struct TCB {// use assertions to check for nullpointers
    // yield
    // 
        TCB* next = nullptr;
        uintptr_t sp;
        uint32_t f = 0;
        TCB* came_from = nullptr;
        bool was = false;
        bool* isready = nullptr;
        Atomic<bool>* locked = nullptr;
        uint32_t* barrier_semaphore = nullptr;
        Queue<TCB, SpinLock>* prev_queue;
        SpinLock* lock;
        SpinLock* bblock = nullptr;
        bool prevbusy = false;
        bool busy = false;
        bool stop = false;
        bool isApit = false;
        uint32_t wakeJiffies = 0;
        virtual ~TCB(){
            
        }
        virtual void doit() = 0;
    };
    static SpinLock* spinlock = new SpinLock();
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
            push(0x200);

            // push(getFlags() | 1<<9);     // eflags

            sp = (uintptr_t) &stack[stack_index+1];
        }
        ~TCBWithWork(){
            // delete &stack;
        }

        void doit() override {
            work();
        }

    };

    extern Queue<TCB, SpinLock> ready_queue;

    extern TCB** current_threads;

    extern TCB* sleep_threads;
    extern SpinLock* sleep_lock;
    extern TCB* me_thread();

    extern "C" void add_and_unlock();

    extern "C" void context_switch(uintptr_t* from, uintptr_t* to);

    extern "C" void context_switchl(uintptr_t* from, uintptr_t* to);

    extern void post_switch();
    

    extern TCB* me_thread();

    extern void helper_doit();


    extern void idle();

    template <typename LockType>
    void block_helper(Queue<TCB, LockType> *q) {
        // if(ready_queue.first != nullptr);
        // ASSERT(Interrupts::isDisabled() == false);


        auto was = Interrupts::disable();
        auto me = current_threads[SMP::me()];
        Interrupts::restore(was);
        // ASSERT(me->busy == true);
        
        // Debug::printf("*** ready_queue empty state: %d\n", ready_queue.first == nullptr);
        auto next = ready_queue.remove();
        //print current thread pointer address
        if (next == nullptr) {
            if (me->bblock != nullptr) {
                me->bblock->unlock();
                me->bblock = nullptr;
            }
            if(!me->isApit) me->busy = false;
            return;
        }
        me->prev_queue = q;
        // ASSERT(Interrupts::isDisabled() == false);
        // me->was = Interrupts::isDisabled();
        next->busy = true;
        current_threads[SMP::me()] = next;
        next->came_from = me;
        context_switch(&me->sp, &next->sp);
        post_switch();
    }
};
// custom block(){

//     while(true)
//     if nothing in thread continue;
//     //avoid bug of 
//     // looping and continously adding a thread to the ready queue because ready_queue is nullptr
// }

extern void stop();
extern void yield();
extern void sleep(uint32_t seconds);

template <typename T>
void thread(T const& f) {
    using namespace impl::threads;
    auto tcb = new TCBWithWork(f);
    ready_queue.add(tcb);
    
}

