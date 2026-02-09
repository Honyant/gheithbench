#include "debug.h"
#include "critical.h"
#include "atomic.h"

static uint32_t counter = 5;


int fib(int n) {
    Debug::printf("*** called fib on %d\n", n);
    if (n <= 1)
        return n;
    return fib(n - 1) + fib(n - 2);
}


void work() {
    Debug::printf("*** hello %d\n", fib(counter));
    Debug::printf("*** counter %d\n", counter );
    counter ++;
}

void work2() {
    critical(work);
}

void work3() {
    critical(work2);
}

/* Called by all cores */
void kernelMain(void) {
    // The C way, use function pointers
    // put one in this variable then give me original value. sequentially consistent
    critical(work3);
}
