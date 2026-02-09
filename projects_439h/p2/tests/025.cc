#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"

/*
 * TLDR: It's a stress test checking that threads waiting at a barrier don't clog up your
 * run queue. Also your yield has to be decently fast.
 *
 * It keeps B threads sitting at a barrier while cycling through N active threads that are
 * counting down for M iterations (which should force them to cycle inefficiently, in reverse
 * queue order). It then makes sure that all the barrier threads actually re-enter the run
 * queue afterwards by using a second barrier.
 */

constexpr int B = 400; // Number of threads waiting at the barrier
constexpr int N = 50; // Number of threads running
constexpr int M = 1250; // Number of loop iterations

Atomic<uint32_t> num {N*M-1};

/* Called by one core */
void kernelMain() {
    Debug::printf("*** hello\n");

    // Set up B threads waiting at a barrier
    auto *b1 = new Barrier{B + 1}, *b2 = new Barrier{B+1};
    for (uint32_t i = 0; i < B; i++) {
        thread([b1, b2] {
            b1->sync();
            b2->sync();
        });
    }

    // Make N threads, alternating counting down M times each
    for (uint32_t i = 0; i < N; i++) {
        thread([i] {
            do {
                while (num.get() % N != i) { yield(); } // Wait for my turn
                if (num.get() % (N * 100) == 0) Debug::printf("*** iter=%d\n", num.get() / N); // Print every 100 cycles
            } while (num.fetch_add(-1) >= N); // Exit after the last cycle
        });
    }

    while (num.get() != (uint32_t) -1) { yield(); } // Wait until counting is done
    Debug::printf("*** done counting\n");
    b1->sync(); // Release the barrier
    Debug::printf("*** barrier released\n");
    b2->sync(); // Make sure all the barrier threads start running again
    Debug::printf("*** done!\n");
}
