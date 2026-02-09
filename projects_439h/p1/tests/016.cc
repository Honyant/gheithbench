#include "debug.h"
#include "critical.h"

static uint32_t counter1 = 0, i = 0, counter2 = 0, sum = 0;

// Checks that recursive critical sections don't cause an infinite stall
// Something already in a critical section should be able to enter new ones
void work1() {
    if (i < 3) {
        Debug::printf("*** core %d ran loop %d\n",counter1, i);
        i++;
        critical(work1);
        i--;
    }
    if (i == 0) {
        Debug::printf("*** core %d says goodbye\n",counter1);
        counter1++;
    }
}

// Checks that processes correctly go from one critical section to another
void work2() {
    sum += counter2;
    counter2++;
    if (counter2 == 4) {
        Debug::printf("*** sum: %d\n", sum);
    }
}

/* Called by all cores */
void kernelMain(void) {
    critical(work1);
    critical(work2);
}
