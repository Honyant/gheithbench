#include "debug.h"
#include "critical.h"
#include "smp.h"
#include "config.h"
#include "atomic.h"
#include "array"

constexpr int SIZE = 100;
static std::array<uint32_t, SIZE> numbers;
static Barrier barrier(kConfig.totalProcs);
static uint32_t final_sum;

/* Called by all cores */
void kernelMain(void) {
    
    // create array
    for(auto i = 0; i < SIZE; i++){
        numbers[i] = i+1;
    }

    // calculate sum of block
    int cpu_id = SMP::me();
    int elements = numbers.size() / kConfig.totalProcs; 
    uint32_t sum = 0;
    for(auto i = cpu_id * elements; i < (cpu_id + 1) * elements; i++){
        sum += numbers[i];
    }

    // calculate final sum
    critical([sum] {
        final_sum += sum;
    });

    // ensure all cores added their sum to the total
    // before printing
    barrier.sync();
    if(SMP::me() == 0) {
        Debug::printf("*** final sum: %d\n", final_sum);
    }

}
