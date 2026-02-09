#include "debug.h"
#include "critical.h"
#include "atomic.h"

static uint32_t counter = 0;
static uint32_t result = 1;

// computes factorial with each core doing a third of the work. tests recursion and successive function calls 
void work() {
    if (counter < 3) {
        counter++;
        result = counter * result;
        if (counter < 3) {
            critical(work);
            return;
        }
        else {
            Debug::printf("*** result 1: %d\n", result);
            return;
        }
    }
    if (counter < 7) {
        counter++;
        result = counter * result;
        if (counter < 7) {
            critical(work);
            return;
        }
        else {
            Debug::printf("*** result 2: %d\n", result);
            return;
        }
    }
    if (counter < 11) {
        counter++;
        result = counter * result;
        if (counter < 11) {
            critical(work);
            return;
        }
        else {
            Debug::printf("*** result 3: %d\n", result);
            return;
        }
    }
}

void kernelMain(void) {   
    critical(work);
}