#include "debug.h"
#include "critical.h"
#include "atomic.h"
#include "config.h"

static uint32_t count = 1;
static uint32_t N = 0;

void counter() {
    critical([] { 
        count *= 2; 
    });
    Debug::printf("*** %d\n",count);
}

void work1() {
    count = 1;
    for (int i = 0; i < 5; i++) {
        critical(counter);
    }
    N++;
    Debug::printf("*** done %d\n",N);
}

void work2() {
    count = 1;
    for (int i = 0; i < 5; i++) {
        critical(counter);
    }
    N++;
    Debug::printf("*** done %d\n",N);
}

void kernelMain(void) {
    critical(work1);
    critical(work2);
}
