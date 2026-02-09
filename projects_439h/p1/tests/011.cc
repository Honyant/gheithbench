#include "debug.h"
#include "critical.h"
#include "atomic.h"

static uint32_t counter = 0;

void work() { // verifies that calling critical multiple times works, as well as calling critical recursively works. If critical is properly locked under all circumstances, this should print up to 100 with everything in proper order
    if(counter > 100) return;
    Debug::printf("*** {%d",counter);
    Debug::printf("...%d}\n",counter);
    counter ++;
    critical(work);
}

/* Called by all cores */
void kernelMain(void) {
    for(int i=0;i<10;i++){
        critical([] {
            work();
            work(); // calling work twice inside a closure
        });
    }
}
