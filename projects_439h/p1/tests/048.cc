#include "debug.h"
#include "critical.h"
#include "atomic.h"

#define SIZE 100

static bool is_prime[SIZE];
static int counter = 1;
static Barrier initial(kConfig.totalProcs);

void initialize() {
    for(int i = 0; i < SIZE; i++) {
        is_prime[i] = true;
    }
}

void mark_false() {
    for (int i = 2; i <= SIZE/counter; i++) {
        is_prime[counter * i - 1] = false;
    }
}

// added this redundant method to test critical recursion
void print_prime() {
    Debug::printf("*** %d\n", counter);
    critical(mark_false);
}


void sieve() {
    while (counter < 100) {
        counter += 1;
        if (is_prime[counter - 1]) {
            critical(print_prime);
        }
    }
}


void kernelMain(void) {
    critical(initialize);
    initial.sync();
    critical(sieve);
}
