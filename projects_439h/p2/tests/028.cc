#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "promise.h"
#include "barrier.h"

Atomic<uint32_t> thread_step{0};
Atomic<uint32_t> main_step{0};
Atomic<uint32_t> triangle{0};

// basic barrier test
void barrierTest(Barrier* b, int n) {
    for (int i = 1; i < n+1; i++) {
        thread([n, b, i] {
            triangle.fetch_add(i); // tests that unique numbers are added to counter2 each time
            b->sync();
        });
    }
}

// barrier and promise test
void promiseBarrierTest(Barrier* outerBarrier, int n, int m) {
    for (int j = 1; j < m+1; j++) {
        Promise<int>* promise = new Promise<int>();
        Barrier* innerBarrier = new Barrier(n);
        for (int i = 0; i < n; i++) {
            if (i == n-1) {
                promise->set(j); // there should be m promises (1 -> m)
            } else {
                thread([promise, n, outerBarrier, innerBarrier] {
                    int value = promise->get();
                    Debug::printf("*** the value is %d\n", value); // should print n-1 times
                    innerBarrier->sync();
                    outerBarrier->sync();
                });
            }
        }
        innerBarrier->sync();
    }
}


/* Called by one core */
void kernelMain(void) {
    int n = 6;
    int m = 3;

    Debug::printf("*** starting test\n");
    Barrier* barrier1 = new Barrier((n-1)*m+1);

    // the first test
    promiseBarrierTest(barrier1, n, m);
    barrier1->sync();
    Debug::printf("*** this should print after all values\n");
    // syncs before the second test begins

    Barrier* barrier2 = new Barrier(n + 1);

    // the second test
    barrierTest(barrier2, n);
    barrier2->sync();
    Debug::printf("*** %dth triangular number: %d\n", n, triangle);

    Debug::printf("*** ending test\n");
}

