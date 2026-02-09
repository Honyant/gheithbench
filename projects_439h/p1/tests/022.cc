#include "debug.h"
#include "critical.h"
#include "atomic.h"

static uint32_t counter1 = 0;
static uint32_t counter2 = 0;
static bool printed = false;

void work() { 
    counter2++;
}

void pink() {
    Debug::printf("*** pink %d\n", counter1);
    counter1++;

    // test recursion
    while (counter1 <= 2) {
        critical(pink);
    }
}

void bow() {
    if (counter2 == 4 && !printed) {
        printed = true;
        Debug::printf("*** bow\n");
    } else if (counter2 > 4) {
        Debug::printf("*** sad\n");
    }
}

/* Called by all cores */
void kernelMain(void) {
    // test that multiple critical sections work
    critical(pink);
    critical(work);
    critical(bow);
}