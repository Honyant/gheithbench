#pragma once

#include "queue.h"
#include "threads.h"

// limitations of given implementation:
//   - single core
//   - no preemption
template <typename T>
class Promise {
    Queue<impl::threads::TCB, SpinLock> waiting{};
    T value{};
    SpinLock* lock = new SpinLock();
    bool isReady = false;
public:
    void set(T const& v) {
        ASSERT(!isReady);
        value = v;
        lock->lock();
        isReady = true;
        auto p = waiting.remove_all();
        lock->unlock();
        while (p != nullptr) {
            auto next = p->next; // why is it a good idea to store p->next in a variable here?
            impl::threads::ready_queue.add(p); // works closely with the thread implementation
            p = next;
        }
    }
    T get() {
        while(true){
            lock->lock();
            if(isReady){
                lock->unlock();
                return value;
            }else{
            auto me = impl::threads::me_thread();
            // me->isready = &isReady;
            // me->lock = lock;
            me->bblock = lock;
            me->busy = true;
            impl::threads::block_helper(&waiting);
            }
        }
        return value;
    }
};



