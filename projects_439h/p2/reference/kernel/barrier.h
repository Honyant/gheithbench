#pragma once



#pragma once

#include "queue.h"
#include "threads.h"

// limitations of given implementation:
//   - single core
//   - no preemption
// template <typename T>
class Barrier {
    Atomic<int32_t> n;
    Queue<impl::threads::TCB, SpinLock> waiting{};
    SpinLock* lock = new SpinLock;

public:
    Barrier(int32_t const n): n(n) {}
    Barrier(Barrier const&) = delete;
    void sync() {
        lock->lock();
        if (n.add_fetch(-1)==0){
            auto p = waiting.remove_all();
            lock->unlock();
            while (p != nullptr) {
                auto next = p->next;
                impl::threads::ready_queue.add(p);
                p = next;
            }
        }else{
            lock->unlock();
            while(n.get()>0){
                impl::threads::current_threads[SMP::me()]->barrier_sephamore = &n;
                impl::threads::current_threads[SMP::me()]->lock = lock;
                impl::threads::block(&waiting);
            }
        }
    }
};