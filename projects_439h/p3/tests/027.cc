#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "blocking_lock.h"
#include "bb.h"
#include "barrier.h"

/*
The goal of this test is mainly to catch any places where you forgot to enable interrupts. 
This includes:
  - after a regular preemption scenario 
  - manually yielding
  - inside a blocking lock
  - after a get/put operation on a bounded buffer
*/

const int N = 120; // this should be a multiple of 20 for the bb test to work correctly

void CHECK_INTERRUPTS_ON() { ASSERT(!Interrupts::isDisabled()); }

/* Called by one core */
void kernelMain(void) {
    Barrier* b = new Barrier(N + 1);
    Debug::printf("*** Checking interrupt status during regular preemption...\n");
    Atomic<int> x{0};
    for (int i = 0; i < N; i++) {
        thread([&x, b] {
            CHECK_INTERRUPTS_ON();
            x.fetch_add(1);
            b->sync();
            CHECK_INTERRUPTS_ON();
        });
    }
    CHECK_INTERRUPTS_ON();
    b->sync();
    CHECK_INTERRUPTS_ON();
    Debug::printf("*** Synced %d threads!\n", x.get());
    ASSERT(x.get() == N);
    Debug::printf("*** SUCCESS!\n\n\n");



    /*
      In this test, we add to x before and after yielding, so it's value should be 2 * N
    */
    Barrier* b2 = new Barrier(N + 1);
    Debug::printf("*** Checking interrupt status when calling yield...\n");
    x.set(0);
    for (int i = 0; i < N; i++) {
        thread([&x, b2] {
            CHECK_INTERRUPTS_ON();
            x.fetch_add(1);
            yield();
            CHECK_INTERRUPTS_ON();
            x.fetch_add(1);
            b2->sync();
            CHECK_INTERRUPTS_ON();
        });
    }
    CHECK_INTERRUPTS_ON();
    b2->sync();
    CHECK_INTERRUPTS_ON();
    Debug::printf("*** Synced %d threads!\n", x.get() / 2);
    ASSERT(x.get() == N * 2);
    Debug::printf("*** SUCCESS!\n\n\n");



    Barrier* b3 = new Barrier(N + 1);
    Debug::printf("*** Checking interrupt status when using a blocking lock...\n");
    int y = 0;
    BlockingLock* bl = new BlockingLock();
    for (int i = 0; i < N; i++) {
        thread([&y, bl, b3] {
            bl->lock();
            CHECK_INTERRUPTS_ON();
            y++;
            bl->unlock();
            CHECK_INTERRUPTS_ON();
            b3->sync();
            CHECK_INTERRUPTS_ON();
        });
    }
    CHECK_INTERRUPTS_ON();
    b3->sync();
    CHECK_INTERRUPTS_ON();
    Debug::printf("*** Synced %d threads!\n", y);
    ASSERT(y == N);
    Debug::printf("*** SUCCESS!\n\n\n");

    Barrier* b4 = new Barrier(N + 1);
    Debug::printf("*** Checking interrupt status when using a bounded buffer...\n");
    BB<int>* bb = new BB<int>(5);
    for (int i = 0; i < N; i++) {
        // create 10 put threads, then 10 get threads, then 10 put threads, etc
        thread([bb, b4, i] {
            if (i % 20 < 10) {
                int x = i;
                bb->put(x);
            } else {
                bb->get();
            }
            CHECK_INTERRUPTS_ON();
            b4->sync();
            CHECK_INTERRUPTS_ON();
        });
    }
    CHECK_INTERRUPTS_ON();
    b4->sync();
    CHECK_INTERRUPTS_ON();
    Debug::printf("*** SUCCESS!\n\n\n");
}
