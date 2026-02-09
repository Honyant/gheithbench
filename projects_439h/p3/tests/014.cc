#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"
#include "blocking_lock.h"

uint32_t threads_total = 10; 
Atomic<uint32_t> intended_thread_id = 0;
Barrier* barrier_1;
BlockingLock* lock; 

/*

this is a test case mainly for your blocking lock

it enforces that only one thread can hold the lock
and increment the intended_thread_id at a time.

*/
void count_up(uint32_t curr_thread) {
    
    // must acquire the lock before proceeding
    lock->lock(); 

    while (intended_thread_id.get() != curr_thread) {
        lock->unlock();  
        yield();         
        lock->lock();    
    }

   
    Debug::printf("*** thread %u\n", curr_thread);
    intended_thread_id.fetch_add(1);

    lock->unlock(); 
    
    barrier_1->sync();
}

void kernelMain(void) {
    Debug::printf("*** start... \n");

    
    barrier_1 = new Barrier(threads_total + 1);

    lock = new BlockingLock();

    intended_thread_id.set(0);      

    for (uint32_t t = 0; t < threads_total; t++) { 
        thread([t] {
            count_up(t);
        });
    }

    barrier_1->sync(); 

    Debug::printf("*** end... \n");
}
