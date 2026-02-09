#include "debug.h"
#include "critical.h"
#include "atomic.h"

static uint32_t counter = 0; 
static uint32_t next_core = -4;

// this case tests support for recursive critical sections

void recursiveCritical() {
    if(++counter % 5 != 0){
        critical(recursiveCritical);
    }
    else{
        Debug::printf("*** Core complete %d\n",SMP::me());
        next_core++; 
    }
}   

void kernelMain(void) {
    Debug::printf("*** Core is awake\n");
    next_core++; 

    while(next_core != SMP::me());
    critical(recursiveCritical); 
}