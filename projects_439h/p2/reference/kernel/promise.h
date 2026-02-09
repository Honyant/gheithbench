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
        while(!isReady){
            impl::threads::current_threads[SMP::me()]->isready = &isReady;
            impl::threads::current_threads[SMP::me()]->lock = lock;
            impl::threads::block(&waiting);
        }
        return value;
    }
};



