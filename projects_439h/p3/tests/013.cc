#include "debug.h"
#include "threads.h"
#include "promise.h"
#include "bb.h"
#include "atomic.h"

/*
* This test computes the first N fibonacci numbers. Each fibonacci number beyond the first two depends on the two preceding numbers, so we use promises to represent these dependencies. 
* Each computation thread waits on promises for the two previous numbers, computes the current number, sets its own promise, and places the result into a bounded buffer. Then,
* a consumer thread retrieves the nubers from the bounded buffer and prints them in order. 
*
* Tests promises and FIFO order of buffer
*/

constexpr int N = 20; // number of Fibonacci numbers to compute

/* Called by one core */
void kernelMain() {
    Debug::printf("*** Starting Fibonacci computation\n");

    // array of promises for fib numbers
    Promise<int>* fibPromises[N];
    BB<int>* fibBB = new BB<int>(N); // bounded buffer for fib numbers
    Atomic<int> running{N - 2};       // number of computation threads running
    Atomic<int> printed{0};           // number of fib numbers printed
    Atomic<int> next_to_put{2};       // next Fibonacci index to put into the buffer

    // initialize the first two Fibonacci numbers
    fibPromises[0] = new Promise<int>();
    fibPromises[1] = new Promise<int>();
    fibPromises[0]->set(0);
    fibPromises[1]->set(1);

    // put first two fib numbers into the bounded buffer
    int value0 = fibPromises[0]->get();
    int value1 = fibPromises[1]->get();
    fibBB->put(value0);
    fibBB->put(value1);

    // first loop: initializes all promises
    for (int i = 2; i < N; i++) {
        fibPromises[i] = new Promise<int>();
    }

    // start computation threads
    for (int i = 2; i < N; i++) {
        thread([i, fibPromises, fibBB, &running, &next_to_put]() {
            // wait for the previous two fib numbers
            int fib_n_1 = fibPromises[i - 1]->get();
            int fib_n_2 = fibPromises[i - 2]->get();

            // compute the current Fibonacci number
            int fib_n = fib_n_1 + fib_n_2;

            // set the promise for the current number
            fibPromises[i]->set(fib_n);

            // wait until it's this thread's turn to put the result into the buffer
            while (next_to_put.get() != i) {
                yield(); // allow other threads to run
            }

            // put the result into the bounded buffer
            fibBB->put(fib_n);

            next_to_put.fetch_add(1);
            running.fetch_add(-1);
        });
    }

    // consumer thread to retrieve and print fib numbers from the bounded buffer
    thread([fibBB, &printed]() {
        for (int i = 0; i < N; i++) {
            int fib_n = fibBB->get();
            Debug::printf("*** Fib(%d) = %d\n", i, fib_n);
            printed.fetch_add(1);
        }
    });

    // wait until all computation threads have finished
    while (running.get() > 0) {
        yield(); // Allow other threads to run
    }

    // wait until all numbers are printed
    while (printed.get() < N) {
        yield(); // allow other threads to run
    }

    Debug::printf("*** Fibonacci computation done\n");
}
