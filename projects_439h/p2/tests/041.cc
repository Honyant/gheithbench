#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"
#include "promise.h"

/*
 * The test case is designed to stress test the synchronization 
 * mechanisms of Promise and Barrier in a multi-threaded environment. 
 * Specifically, it creates a series of threads that set and retrieve 
 * promises in a non-deterministic order, simulating an edge case where 
 * the completion times of threads are unpredictable.

 * Each thread sets a promise with a unique value, and another 
 * set of threads retrieves these values and sums them up. A barrier 
 * is used to ensure that all threads complete their operations before 
 * the main thread proceeds. The test verifies that the sum of all 
 * promise values is correct, ensuring that the promises and barriers 
 * are functioning as expected under these conditions.
 */
void kernelMain(void) {
    Debug::printf("*** Edge Case Test started.\n");

    const int numPromises = 10;
    Promise<int> promises[numPromises];
    Barrier barrier{numPromises + 1}; // Synchronize all threads plus the main thread

    // Using pointers for Atomic<int>
    Atomic<int>** results = new Atomic<int>*[numPromises];
    for (int i = 0; i < numPromises; ++i) {
        results[i] = new Atomic<int>(0); // Initialize each element to 0
    }

    Atomic<int> sum{0};

    // Create threads that will set promises in a non-deterministic order
    for (int i = 0; i < numPromises; ++i) {
        thread([&, i] {
            Debug::printf("Thread %d is working...\n", i);
            for (int j = 0; j < 1000000; ++j) {
                if (j % (100000 / (i + 1)) == 0) {
                    yield(); 
                    Debug::printf("Thread %d yielding...\n", i);
                }
            }
            Debug::printf("Thread %d setting promise to %d\n", i, i * 10);
            promises[i].set(i * 10);
        });
    }

    // Create threads that will retrieve the promise values and sum them up
    for (int i = 0; i < numPromises; ++i) {
        thread([&, i] {
            int result = promises[i].get();
            Debug::printf("*** Some thread received promise value\n");
            results[i]->set(result);
            sum.add_fetch(result);
            barrier.sync();
        });
    }

    Debug::printf("Main thread waiting for all threads to sync...\n");
    barrier.sync(); // Ensure all threads have completed their work

    Debug::printf("*** All threads synchronized.\n");

    // Check if the sum is as expected (sum of multiples of 10 from 0 to 90)
    int expectedSum = 0;
    for (int i = 0; i < numPromises; ++i) {
        expectedSum += i * 10;
    }

    if (sum.get() == expectedSum) {
        Debug::printf("*** Edge Case Test passed! Sum = %d\n", sum.get());
    } else {
        Debug::printf("*** Edge Case Test failed! Expected sum = %d, but got %d\n", expectedSum, sum.get());
    }

    Debug::printf("*** Edge Case Test completed.\n");

    // Clean up dynamically allocated memory
    for (int i = 0; i < numPromises; ++i) {
        delete results[i];
    }
    delete[] results;
}
