#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "blocking_lock.h"
#include "barrier.h"

int32_t THREADS = 100;
Atomic<int32_t> count{0};

/* Called by one core */
void kernelMain(void) {
    ASSERT(!Interrupts::isDisabled());
    Debug::printf("*** Test 1: many threads all trying to access heap\n");

    for (int32_t i=0; i<THREADS; i++) {
        ASSERT(!Interrupts::isDisabled());
        thread([i] {
            thread([] {
                stop();
            });
        });
        count.add_fetch(1);
        ASSERT(!Interrupts::isDisabled());
    }

    while (count.get() < THREADS) ASSERT(!Interrupts::isDisabled());
    Debug::printf("*** Test 1 passed\n");

    // tests having an efficient way to wake up threads
    // need a reaper/helper thread (can't have next thread do it)
    Debug::printf("*** Test 2: sleep helper thread for O(1)\n");
    auto barrier = new Barrier(THREADS + 1);
    for (int32_t i=0; i<THREADS; i++) {
        ASSERT(!Interrupts::isDisabled());
        thread([barrier] {
            sleep(2);
            barrier->sync();
        });
        ASSERT(!Interrupts::isDisabled());
    }

    // do you have a way to wake up threads when all are sleeping?
    sleep(4);
    barrier->sync();
    Debug::printf("*** Test 2 passed\n");


    Debug::printf("*** Test 3: Quick test of Blocking Lock one more time\n");
    barrier = new Barrier(THREADS + 1);
    auto bl = new BlockingLock();
    for (int32_t i=0; i<THREADS; i++) {
        ASSERT(!Interrupts::isDisabled());
        thread([bl, barrier] {
            bl->lock();
            bl->unlock();
            barrier->sync();
        });
    }
    Debug::printf("*** Test 3 passed\n");
    barrier->sync();
}

