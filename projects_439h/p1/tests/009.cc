#include "critical.h"
#include "atomic.h"
#include "debug.h"

#define SZ 5

// vector dot product - just a sample program, not very parallelizable because of shared resources (dot and pos)
// this test case contains no nested critical() calls, but some (incorrect) adaptations to nested criticals cause it to fail
static bool done = 0;
static int* v = new int[SZ]{0,1,2,3,4};
static int* w = new int[SZ]{4,3,2,1,0};
static int dot = 0;
static int pos = 0;

void work(void) {
    if (done) { // multiple cores may enter work after the computation is done but only one should print the value
    	return;
    }
    if (pos == SZ) {
        delete v; // clean up resources
        delete w;
        Debug::printf("*** %d\n", dot); // print result ("10")
        done = 1;
    }
    else { 
        dot += v[pos] * w[pos];
        pos++;
    }
}

/* Called by all cores */
void kernelMain(void) {
    while (!done) { // check on done is not in the critical section
        critical(work);
    }
}

