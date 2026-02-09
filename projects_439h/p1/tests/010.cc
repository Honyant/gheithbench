#include "debug.h"
#include "critical.h"
#include "atomic.h"
#include "random.h"

/*
    This algorithm finds the number of trailing zeros in the factorial of a number.
    The reason it doesn't cause a race condition is because the sum is only printed 
    when the last core has finished running.
*/
static Random * rng = new Random(1234);
static uint64_t sum = 0;
static uint32_t iter = 0;

void trailingZeros() {
    double rand = (rng->next() % 1234);

    double den = 5;
    while (den <= rand) {
        sum += rand / den;
        den *= 5;
    }

    iter++;
}

/*
Pay attention to the shared resource and why it changes the way it does.
*/
static uint32_t depth = 0;

void work() {
    depth++;
    Debug::printf("*** I hope this doesn't lock forever %d\n", depth);
    critical([]() {
        depth++;
        Debug::printf("*** I can still run right? %d\n", depth);
        depth--;
    });
    Debug::printf("*** YAY IT WORKED, %d\n", depth);
    depth--;
}

/* Called by all cores */
void kernelMain(void) {
    // The C way, use function pointers
    critical(work);

    critical(trailingZeros);

    if (iter == 4) critical([]{Debug::printf("*** sum = %d\n", sum);});
}