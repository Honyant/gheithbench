#pragma once

#include "atomic.h"
#include "queue.h"
#include "heap.h"
#include "debug.h"
#include "smp.h"

constexpr size_t STACK_BYTES = 8 * 1024;
constexpr size_t STACK_WORDS = STACK_BYTES / sizeof(uintptr_t);  /* sizeof(word) == size(void*) */

namespace impl::threads {

    // Limitations of current implementation:
    //    - assumes single core
    //    - no preemption
    
    struct TCB {
        TCB* next = nullptr;
        uintptr_t sp;
        TCB* came_from = nullptr;
        bool* isready = nullptr;
        Atomic<int32_t>* barrier_sephamore = nullptr;
        Queue<TCB, SpinLock>* prev_queue;
        SpinLock* lock;
        bool stop = false;
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

    extern TCB** prev_threads;

    extern "C" void context_switch(uintptr_t* from, uint32_t to);

    template <typename LockType>
    void block(Queue<TCB, LockType> *q) {
        
        auto next = ready_queue.remove();
        if (next == nullptr) return;

        auto me = current_threads[SMP::me()];
        me->prev_queue = q;
        current_threads[SMP::me()] = next;
        prev_threads[SMP::me()] = me;

        context_switch(&me->sp, next->sp);
        auto& g = prev_threads[SMP::me()];
        if(g->stop){
            delete g;
        }else{
            if(g->isready!=nullptr){
                if(!*g->isready){
                    g->lock->lock();
                        if(!*g->isready) g->prev_queue->add(g);
                        else ready_queue.add(g);
                    g->lock->unlock();
                }else ready_queue.add(g);
                g->isready = nullptr;
            }else if(g->barrier_sephamore!=nullptr){
                if(g->barrier_sephamore->get()>0){
                    g->lock->lock();
                        if(g->barrier_sephamore->get()>0)  g->prev_queue->add(g);
                        else ready_queue.add(g);
                    g->lock->unlock();
                }else ready_queue.add(g); 
                g->barrier_sephamore = nullptr;
            }
            else{
                g->prev_queue->add(g);
            }
        }
        //double checking

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

