#pragma once

#include <stdint.h>

#include "atomic.h"
#include "debug.h"
#include "queue.h"
#include "threads.h"

class Semaphore {
    private:
    int counter;
    Queue<impl::threads::TCB, NoLock> q{};
    SpinLock s{};
public:

    Semaphore(uint32_t const n) {
        counter = n;
    }

    void signal() {
        s.lock();
        counter++;
        if(counter <= 0){
            auto next = q.remove();
            s.unlock();
            if (next != nullptr) impl::threads::ready_queue.add(next);
            return;
        }
        s.unlock();
        
        // if(next != nullptr) {
        //     Debug::printf("next is not null, freeing from queue %p\n", &q);
        // }
        // else Debug::printf("next is null at queue %p\n", &q);   
    }

    void wait() {
        auto me = impl::threads::me_thread();
        do{
            me->busy = false;
            s.lock();
            if(counter > 0){ 
                counter--;
                s.unlock();
                return;
            }
            s.unlock();
            me->busy = true;
        }
        while (!impl::threads::blocknl(&q, &counter, &s));
        me->busy = false;
    }
};