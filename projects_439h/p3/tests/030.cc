#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"



// This test checks that your sleep implemetation works at scale.
// Multiple threads will be woken up on the same apit clock tick,
// and the threads should be woken in an offline interrupt
// handler instead of in the apit handler loop. They should then
// be put into a queue for offline execution.

const uint32_t n = 400;
Barrier barrier(n + 1);

/* Called by one core */
void kernelMain(void) {
    Debug::printf("*** hello\n");
    uint32_t sum = 0;

    for (uint32_t i = 0; i < n; i++) {
        thread([&sum] {
            sleep(1);
            sum = sum + 1;
            barrier.sync();
        });
    }

    barrier.sync();
    Debug::printf("*** sum = %u\n", sum);
    Debug::printf("*** done\n");

}

