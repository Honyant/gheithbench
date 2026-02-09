#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"
#include "promise.h"

/*
This test case primarily is a stress test in which I am creating a
large number of threads in order to see if you context switch fast enough.
In terms of promises and barriers, this uses them in a very simple
implementation. Good luck!
*/

constexpr uint32_t THREAD_COUNT = 500;

void kernelMain(void)
{
    Debug::printf("*** Welcome to my test case\n");
    auto barrier = new Barrier(THREAD_COUNT + 1);

    Promise<int> *results[THREAD_COUNT];
    for (uint32_t i = 0; i < THREAD_COUNT; i++)
    {
        results[i] = new Promise<int>();
    }

    for (uint32_t i = 0; i < THREAD_COUNT; i++)
    {
        thread([i, barrier, results]
               {
            int computationResult = (i + 1) * 10;  // Each thread computes a value based on its index
            Debug::printf("*** A thread is computed. Let's hope you keep your promises...\n");

            // Fulfill the promise with the computed result
            results[i]->set(computationResult);

            barrier->sync(); });
    }

    // Wait for all threads to complete their computations
    barrier->sync();

    int totalSum = 0;
    for (uint32_t i = 0; i < THREAD_COUNT; i++)
    {
        totalSum += results[i]->get();
    }

    Debug::printf("*** Total sum of computations is: %d\n", totalSum);

    Debug::printf("*** Lavender confetti for everyone\n");
}
