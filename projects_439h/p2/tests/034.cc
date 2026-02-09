#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "promise.h"
#include "barrier.h"

constexpr uint32_t N = 10; // Number of threads to test
constexpr uint32_t expected_sum = (N * (N + 1)) / 2; // Expected sum if promises are working correctly

Atomic<uint32_t> global_sum = 0; // Atomic variable to accumulate results from all threads

void testPromises() {
    Debug::printf("*** Starting Promise Test\n");

    // Create barriers for synchronizing threads
    auto start_barrier = new Barrier(N + 1); // Barrier to ensure all threads start at the same time
    auto start_barrier_2 = new Barrier(N + 1); // Barrier to avoid reusing race condition
    auto end_barrier = new Barrier(N + 1); // Barrier to ensure all threads finish before checking results

    // Create arrays for promises
    auto promises1 = new Promise<uint32_t>[N];
    auto promises2 = new Promise<uint32_t>[N];

    for (uint32_t i = 1; i <= N; ++i) {
        thread([i, &promises1, &promises2, start_barrier, start_barrier_2, end_barrier] {
            // Wait for all threads to be ready
            start_barrier->sync();

            // Set values on promises
            promises1[i - 1].set(i);
            promises2[i - 1].set(i * 2);

            // Ensure all threads have set their values
            start_barrier_2->sync();

            // Get values from promises
            uint32_t value1 = promises1[i - 1].get();
            uint32_t value2 = promises2[i - 1].get();

            //Debug::printf("*** Thread %d got values: %d and %d\n", i, value1, value2);

            // Accumulate results to global_sum
            global_sum.fetch_add(value1);
            global_sum.fetch_add(value2);

            // Wait for all threads to complete their operations
            end_barrier->sync();
        });
    }

    // Ensure all threads are started before proceeding
    start_barrier->sync();

    // New start barrier to avoid race condition
    start_barrier_2->sync();

    // Ensure all threads complete their operations before checking results
    end_barrier->sync();

    // Check if the accumulated sum matches the expected result
    uint32_t result = global_sum.get();
    uint32_t expected_result = (expected_sum * 3); // Sum of values from both promises
    if (result == expected_result) {
        Debug::printf("*** All promises worked correctly. Sum: %d\n", result);
    } else {
        Debug::printf("*** Error: Accumulated sum %d does not match expected sum %d\n", result, expected_result);
    }

    // Clean up bad
}

void kernelMain(void) {
    Debug::printf("*** Kernel Main Started\n");
    testPromises();
    Debug::printf("*** Kernel Main Finished\n");
}
