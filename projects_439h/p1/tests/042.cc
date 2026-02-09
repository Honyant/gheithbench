#include "debug.h"
#include "critical.h"
#include "atomic.h"

static uint32_t counter = 0;
static uint32_t counter2 = 0;
Barrier barrier(4);
Barrier barrier2(4);
Atomic<bool> first_free { true };

void work2() {
    Debug::printf("*** inside %d\n", counter2);
    counter2++;
}

void work() {
    Debug::printf("*** outside %d\n",counter);
    counter++;
    critical(work2);
}

//testing simple nested critical sections

void kernelMain(void) {
    // The C way, use function pointers
    critical(work);

    //want all cores to wait when they get here
    barrier.sync();

    if (first_free.exchange(false)) {
        Debug::printf("***\n");
        counter = 0;
        counter2 = 0;
    }

    //barrier's not reusable
    barrier2.sync();

    // The C++ way, use closures
    // it's the exact same logic as the first part of this test case
    critical([] {
        Debug::printf("*** outside2 %d\n",counter);
        counter++;
        critical([] {
            Debug::printf("*** inside2 %d\n", counter2);
            counter2++; });
    });
}
