#include "debug.h"
#include "critical.h"
#include "atomic.h"

static uint32_t counter = 0;
static int total_sum = 0;
static const int sumRangeSize = 10000;
static const int expected_sum = sumRangeSize * 2 * (sumRangeSize * 4 - 1);

void done() {
    if (total_sum == expected_sum) {
        Debug::printf("*** Yay we added numbers in parallel! The total sum is: %d\n", total_sum);
    }
}
// Called by all cores
void kernelMain(void) {
    int local_counter = 0;
    critical([&local_counter]() {
        local_counter = counter;
        counter++;
        Debug::printf("*** Core %d will sum the numbers from %d to %d (inclusive)\n", local_counter, local_counter * sumRangeSize, local_counter * sumRangeSize + sumRangeSize - 1);
    });

    //for subsequent code each thread will have a different local_counter

    int local_sum = 0;
    for (int i = local_counter * sumRangeSize; i <= local_counter * sumRangeSize + sumRangeSize - 1; i++) {
        local_sum += i;
    }

    critical([local_sum, local_counter]() {
        total_sum += local_sum;
        critical(done); //tests for no deadlock for a core using critical while in critical
    });
}