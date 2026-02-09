#include "debug.h"
#include "critical.h"
#include "atomic.h"

static uint32_t counter = 0;
static Barrier firstBarrier(kConfig.totalProcs);
static uint32_t coreNum = 0;


void lastWork() {
    Debug::printf("*** core %d: bye\n", coreNum);
    coreNum++;
}

void work2() {
    Debug::printf("*** counter: %d\n",counter);
}

// for each counter do the work
void work() {
    int x = 5;
    // loop 5 times and print which loop we are on
    while (x != 0) {
        critical(work2);
        Debug::printf("*** count: %d\n", x);
        x--;
    }
    counter++;
}

/* Called by all cores */
void kernelMain(void) {
    // The C way, use function pointers

    // we should be able to use multiple because no matter what order they go in the output won't change (global vars incremented)
    critical(work);
    critical(work);
    critical(work);
    critical(work);

    firstBarrier.sync();

    critical(lastWork);
}
