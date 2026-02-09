#include <stdint.h>
#include "smp.h"
#include "debug.h"
#include "config.h"
#include "threads.h"
#include "bb.h"
#include "barrier.h"
#include "atomic.h"
#include "blocking_lock.h"

// this tests bounded buffers and blocking locks.

void kernelMain(void) {

    int n = 10; // size of the buffer

    // bounded buffer test: get 10 values, put 20 values, then get 10 values
    BB<int>* buffer = new BB<int>(n);
    Barrier* barrier = new Barrier(n+1);
    Atomic<int> sum = 0;
    // get values 1 -> n in the buffer
    for (int i = 1; i <= n; i++) {
        thread([buffer, barrier, &sum] {
            int val = buffer->get();
            sum.add_fetch(val);
            barrier->sync();
        });
    }
    // put values 1 -> 2n in the buffer, in decreasing order
    for (int i = 2*n; i >= 1; i--) {
        int val = i;
        buffer->put(val);
    }
    barrier->sync();
    Debug::printf("*** sum of numbers from %d to %d: %d\n", n+1, n*2, sum.get());

    Barrier* barrier2 = new Barrier(n + 1);
    Atomic<int> sum2 = sum.get();

    // get the remaining values from the buffer
    for (int i = 1; i <= n; i++) {
        thread([buffer, barrier2, &sum2] {
            int val = -1 * buffer->get();
            sum2.add_fetch(val);
            barrier2->sync();
        });
    }
    barrier2->sync();

    Debug::printf("*** subtract numbers from %d to %d: %d\n", 1, n, sum2.get());

    // blocking lock test: same as last part of last test, but using nonatomic variable
    BlockingLock* blockingLock = new BlockingLock();
    Barrier* barrier3 = new Barrier(n + 1);
    int* sumNotAtomic = new int(sum.get());
    for (int i = 1; i <= n; i++) {
        thread([blockingLock, barrier3, sumNotAtomic, i] {
            blockingLock->lock();
            *sumNotAtomic -= i;
            blockingLock->unlock();
            Debug::printf("*** unlocked\n");
            barrier3->sync();
        });
    }
    barrier3->sync();

    Debug::printf("*** same as before: %d\n", *sumNotAtomic);

    Debug::printf("*** done\n");
}

