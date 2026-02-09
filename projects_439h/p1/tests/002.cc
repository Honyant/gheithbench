#include "debug.h"
#include "critical.h"
#include "atomic.h"

// this test case just uses critical to schedule the 4 cores to compute the 10000th fib number
// it also has a very simple case of calling critical within critical at the end to check that 
// all the work is done

static int ix = 0;
static long long fib[10000];

void check() {
    if (ix == 10000) {
        Debug::printf("*** Fib at 10000: %d\n", fib[ix - 1]);
    }
}

void work() {
    if (ix < 2) fib[ix] = 1;
    else fib[ix] = fib[ix - 1] + fib[ix - 2];
    ix++;
    critical(check);
}


/* Called by all cores */
void kernelMain(void) {
    // The C way, use function pointers
    Debug::printf("*** Calculating Fib sequence:\n");
    for (int i = 0; i < 2500; i++) {
        critical(work);
    }
}
