#include "debug.h"
#include "critical.h"
#include "atomic.h"

/**
 * This test case tests:
 * a) exchanging control of the critical section from one core to another
 * b) calls to critical from within a critical section
*/

static int i = 0;
static int x = 0;
static const char* arr1 [] = {"\\(^O^)/", "(>w<)", "(-_-;)", "(=^・^=)"};
static const char* arr2 [] = {"woohoo", "yippee", "hooray", "yay"};
static Barrier barrier(kConfig.totalProcs);

void hello2() {
    Debug::printf("*** %s\n", arr2[i]);
}

void hello1() {
    Debug::printf("*** %s\n", arr1[i]);
    // check calling critical within a critical section
    // this part should happen once for each core
    critical(hello2);
    i++;
}

void recursion() {
    if(x < 4) {
        // check recursion
        // this should happen four times for each core
        Debug::printf("*** %s %s\n", arr2[i - kConfig.totalProcs], arr1[x]);
        x++;
        critical(recursion);
    }
    else {
        x = 0;
        i++;
    }
}

void kernelMain(void) {
    critical(hello1);
    barrier.sync();
    critical(recursion);
}
