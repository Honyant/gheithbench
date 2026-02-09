#include "debug.h"
#include "critical.h"
#include "atomic.h"

static uint32_t counter = 1;

//Can't use hashmaps since this is freestanding c++ so I just made a giant array
static int collatz_cache[10000] = {0,1};

// Recursive function to calculate Collatz length
int collatz_length(int n, int iterations) {
    if (collatz_cache[n]!=0) {
        Debug::printf("*** Took %d iterations\n" , iterations);
        return collatz_cache[n];
    }
    
    if (n%2 == 0) {
        return 1 + collatz_length(n/2, iterations+1);
    } 
    else {
        return 1 + collatz_length(3*n+1, iterations+1);
    }

}

void wipe_cache(){
    for (int i=2; i<10000; i++) {
        collatz_cache[i] = 0;
    }
}

void work() {
    for (int i=0; i<25; i++){
        Debug::printf("*** Calculating collatz length of %d\n" , counter);
        int length = collatz_length(counter, 0);
        collatz_cache[counter] = length;
        Debug::printf("*** Collatz length is %d\n", length);
        counter ++;
    }
    wipe_cache();
}


void kernelMain(void) {
    critical(work);
    critical(work);
}
