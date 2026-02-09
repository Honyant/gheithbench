#include "debug.h"
#include "threads.h"
#include "barrier.h"
#include "pit.h"

constexpr uint32_t threads = 300;
constexpr uint32_t ms_tolerance = 2;

Barrier guard{threads + 1};

/* Called by one core */
void kernelMain() {
    Debug::printf("*** Checking that threads are sleeping accurately...\n");

    for (uint32_t i = 0; i < threads; i++) {
        thread([i] {
            auto start = Pit::jiffies;
            sleep(1);
            auto end = Pit::jiffies;
            auto elapsed = end - start;

            Debug::printf("| [%d] Elapsed time: %.3fms -- variance %dms\n", i, elapsed, elapsed - 1000); // yeah ik about overflows
            if (elapsed < 1000 - ms_tolerance || elapsed > 1000 + ms_tolerance) {
                Debug::printf("*** Iteration %d sleep timeout was outside of %dms tolerance, took %dms.\n", i, ms_tolerance, elapsed);
            }

            guard.sync();
        });
    }

    guard.sync();

    Debug::printf("*** done! :3\n");
}
