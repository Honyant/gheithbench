#include "debug.h"
#include "critical.h"
#include "atomic.h"

static Barrier barrier1(kConfig.totalProcs);
static Barrier barrier2(kConfig.totalProcs);
static Barrier barrier3(kConfig.totalProcs);
static Barrier barrier4(kConfig.totalProcs);
static Barrier barrier5(kConfig.totalProcs);
static Barrier barrier6(kConfig.totalProcs);
static uint32_t counter = 0;
static uint32_t sum = 0;

void print() {
    Debug::printf("*** %d\n", ++counter);
}

void recurse(uint32_t num) {
    if (num > 1) {
        recurse(num - 1);
    }

    sum += num;
}

void recurse_critical(uint32_t num) {
    if (num > 1) {
        critical([num] {
            recurse_critical(num - 1);
        });
    }

    sum += num;
}

void kernelMain(void) {
    // Test sequential critical sections
    for (int i = 0; i < 3; i++) {
        critical(print);
    }

    barrier1.sync();
    
    // Test nested critical sections
    critical([] {
        critical(print);
        critical(print);
    });

    barrier2.sync();

    // Test nested critical sections with statement in between
    critical([] {
        critical(print);
        Debug::printf("*** spacer\n");
        critical(print);
    });

    barrier3.sync();

    // Test recursive function with external critical section
    critical([] {
        recurse(100); 
    });
    barrier4.sync();
    Debug::printf("*** sum: %d\n", sum);

    barrier5.sync();

    // Test recursive function with internal critical section
    recurse_critical(100); 
    barrier6.sync();
    Debug::printf("*** sum: %d\n", sum);
}
