#include "debug.h"
#include "critical.h"
#include "atomic.h"

static uint32_t counter = 0;
void work2();

/**
 * This test tests what happens when two functions
 * call each other. 
 */

void work1() {
    Debug::printf("*** hello %d\n",counter);
    counter ++;
    if (counter < 6) {
        critical(work2);
    } else {
        Debug::printf("*** from work 1\n");
    }
}

void work2() {
    Debug::printf("*** goodbye %d\n", counter);
    counter ++;
    if (counter < 6) {
        critical(work1);
    } 
}

/* Called by all cores */
void kernelMain(void) {
    critical(work1);
}
