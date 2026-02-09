#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"


// This test cased is designed to test that you accurately free all data associated with a thread when it has called stop.
// It is intentionally very simple to allow for ease of debugging.
// To pass this test, you need to free stopped threads and have a functional implementation of barrier.

constexpr uint32_t threadsPerRepetition = 100;
constexpr uint32_t totalRepetitions = 20;

Atomic<uint32_t> count = 0;


/* Called by one core */
void kernelMain(void) {
    Debug::printf("*** Test Start!\n");


    for (uint32_t i = 0 ; i < totalRepetitions; i++){
        // Each repetition, create a number of threads that just sync then stop
        // We need a new barrier for each repetition
        auto barrier = new Barrier(threadsPerRepetition + 1);
        for (uint32_t a=1; a<=threadsPerRepetition; a++) {
            thread([barrier] {
                barrier->sync();
                stop();
            });
        }

        // Syncing here ensures us that we will not proceed to the next iteration until all of the threads have synced up and are ready to call stop
        barrier->sync();
        Debug::printf("*** Got through %d iterations!\n", i);
    }

    Debug::printf("*** Test Complete!\n");

}
    