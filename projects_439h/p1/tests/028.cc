#include "debug.h"
#include "critical.h"
#include "atomic.h"

//DINING PHILOSOPHERS PROBLEM (simplified)
static uint32_t counter = 0;
static Atomic<bool> fork{ false };
static uint32_t cnt = 0;
static Atomic<int> curNum = 1;

static Barrier barrier(kConfig.totalProcs);
static Barrier barrier2(kConfig.totalProcs);

void idle() {
    //avoiding optimizing it away
    volatile int loops = 50000000;
    
    while (loops != 0) {
        loops = loops - 1;
    };
}

void eat() {
    if (fork.get()) 
        Debug::printf("*** Uh oh... this shoulda never happened!");
    fork = true;
    
    //If your implementation of critical allows more than one core, then this will cause
    //output of Uh oh... this shoulda never happened!
    idle();
    fork = false;
    cnt++;
}

void dine() {
    eat();
    Debug::printf("*** Someone finished eating!! %d\n",counter);
}

//enough waiting time that any interweaving of cores would lead to incorrect output
void work2() {
    idle();
    critical([] {critical(dine);});
}

void computeSum() {
    //If there is only one core, proceed as usual
    uint32_t inc = 10;
    for (uint32_t i = counter; i < counter + inc; i++) {
        Debug::printf("*** %d\n", i);
    }
    //alternate between cores
    counter+=inc;
}

/* Called by all cores */
void kernelMain(void) {
    // The C way, use function pointers
    critical(dine);
    barrier.sync();
    if (kConfig.totalProcs < cnt) {
        Debug::printf("*** How did u manage to call eat more times than necessary?\n");
    } else {
        Debug::printf("*** Passed!\n" );
    }
    critical(work2);
    
    //checks critical works in loops
    barrier2.sync();
    for (int i = 0; i < 4; i++) {
        critical(computeSum);
    }
}
