#include "debug.h"
#include "smp.h"
#include "config.h"
#include "atomic.h"
#include "array"
#include "barrier.h"
#include "promise.h"

constexpr int SIZE = 10000;
constexpr int THREADS = 100;
static Barrier barrier(THREADS + 1);


void barrier_test(uint32_t* numbers) {
    uint32_t* sums = new uint32_t[SIZE];

    // calculate sum of blocks
    for(int i = 0; i < THREADS; i++) {
        uint32_t thread_id = i;
        thread([thread_id, numbers, sums] {
            int elements = SIZE / THREADS; 
            uint32_t sum = 0;
            for(auto i = thread_id * elements; i < (thread_id + 1) * elements; i++){
                sum += numbers[i];
            }
            sums[thread_id] = sum;
            barrier.sync();
        });
    }
    
    // ensure all threads calculated their sum
    // before getting the final sum and printing
    barrier.sync();

    uint32_t final_sum = 0;
    for(int i = 0; i < THREADS; i++) {
        final_sum += sums[i];
    }
    
    Debug::printf("*** final sum with Barriers: %d\n", final_sum);

    delete [] sums;
}

void promise_test(uint32_t* numbers) {
    Promise<uint32_t>** sums = new Promise<uint32_t>*[THREADS];
    for(int i = 0; i < THREADS; i++)
        sums[i] = new Promise<uint32_t>();

     // calculate sum of blocks
    for(int i = 0; i < THREADS; i++) {
        uint32_t thread_id = i;
        thread([thread_id, numbers, sums] {
            int elements = SIZE / THREADS; 
            uint32_t sum = 0;
            for(auto i = thread_id * elements; i < (thread_id + 1) * elements; i++){
                sum += numbers[i];
            }

            sums[thread_id]->set(sum);

        });
    }
    
    // use promises to retrieve sums from threads
    volatile uint32_t final_sum = 0;
    for(int i = 0; i < THREADS; i++)
        final_sum += sums[i]->get();
    Debug::printf("*** final sum with Promises: %d\n", final_sum);

    // free heap memory
    for(int i = 0; i < THREADS; i++)
        delete sums[i];
    delete [] sums;
}

/* Called by all cores */
void kernelMain(void) {
    uint32_t* numbers = new uint32_t[SIZE];

    // create array
    for(auto i = 0; i < SIZE; i++){
        numbers[i] = i+1;
    }

    // run tests
    barrier_test(numbers);
    promise_test(numbers);


    delete [] numbers;
}