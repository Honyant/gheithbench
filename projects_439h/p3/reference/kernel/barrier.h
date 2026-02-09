#pragma once

#include "queue.h"
#include "threads.h"

// limitations of given implementation:
//   - single core
//   - no preemption
// template <typename T>
class Barrier {
    uint32_t n;
    Queue<impl::threads::TCB, SpinLock> waiting{};
    SpinLock* lock = new SpinLock();

public:
    Barrier(uint32_t const n): n(n) {}
    Barrier(Barrier const&) = delete;
    void sync() {
        // Debug::printf("Barrier::sync() n=%d\n", n.get(), SMP::me());
        lock->lock();
        n-=1;
        // Debug::printf("Barrier::sync() n=%d\n", n);
        if(n==0){
            auto p = waiting.remove_all();
            lock->unlock();
            while (p != nullptr) {
                auto next = p->next;
                impl::threads::ready_queue.add(p);
                p = next;
            }
            return;
        }else{
            lock->unlock();
        }
        while(n>0){
            lock->lock();
            if(n==0){
                lock->unlock();
                return;
            }
            auto me = impl::threads::me_thread();
            // me->barrier_semaphore = &n;
            // me->lock = lock;
            me->bblock = lock;
            me->busy = true;
            // ASSERT(impl::threads::me_thread()->busy);
            impl::threads::block_helper(&waiting);
        }
    }
};