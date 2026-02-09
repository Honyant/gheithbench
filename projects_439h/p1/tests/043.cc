#include "debug.h"
#include "critical.h"
#include "atomic.h"

static uint32_t core_counter = 0;
static uint32_t second_layer_counter = 0;

void work2() {
    Debug::printf("*** Current index in for loop: %d\n", second_layer_counter);
}

void work1() {
    
    Debug::printf("*** Current core: %d\n", core_counter);
    for(int i = 0; i < 5; i ++) {
        critical(work2);
        second_layer_counter ++;
    }
    
    second_layer_counter = 0; // reset the counter
    core_counter ++; // increment the core counter
}

/* Called by all cores */
void kernelMain(void) {
    critical(work1);
}
