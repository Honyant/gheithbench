#include "debug.h"
#include "critical.h"
#include "atomic.h"

// Simple test to verify that critical sections can run within critical sections

volatile static int num_prints = 0;

void work() {
    if (num_prints < 6) {
        Debug::printf("*** running %d\n", num_prints);
        num_prints = num_prints + 1;
        critical(work);
    }
}

/* Called by all cores */
void kernelMain(void) {
    critical(work);
}
