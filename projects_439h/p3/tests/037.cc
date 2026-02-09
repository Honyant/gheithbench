#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "blocking_lock.h"
#include "barrier.h"


/* Called by one core */
void kernelMain(void) {
    Debug::printf("*** hello\n");
    BlockingLock* block_lock = new BlockingLock();
    auto thread_counter = new Atomic<uint32_t>(0);
    auto sync_check = new Atomic<uint32_t>(0);

    Barrier* b0 = new Barrier(2);
    Barrier* b1 = new Barrier(101);

    // Blocking lock test

    for(int i  = 0; i < 100; i++){
        thread([block_lock, thread_counter, b1, b0, sync_check]{
            sync_check->add_fetch(1);
            if(sync_check->get() == 50){
                b0->sync();
            }
            block_lock->lock();

            if (sync_check->get() < 99){
                b0->sync();
                sync_check->set(100);
            }

            thread_counter->fetch_add(1);
            block_lock->unlock();
            b1->sync();
        });
    }
    b1->sync();
    Debug::printf("*** Counted %d Threads\n", thread_counter->get());

}

