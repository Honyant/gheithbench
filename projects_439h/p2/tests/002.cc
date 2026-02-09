#include <stdint.h>
#include "smp.h"
#include "debug.h"
#include "config.h"
#include "threads.h"
#include "promise.h"

constexpr int N = 10;

/* Called by one core */
void kernelMain(void) {
    // We will try to compute the N + 1th fibonacci number
    // This code is not memoized so we can create many threads!

    Promise<int>* answers[1024];
    for (int i = 0; i < 1024; i++) {
        answers[i] = new Promise<int>();
    }


    auto fibonacci = [&](auto& self, int n, int id) -> void {
        // If N <= 1, resolve the promise with the Fibonacci value
        if (n <= 1) {
            // Debug::printf("*** Setting value at id %d\n", id);
            answers[id]->set(1);
            stop();  // Stop thread execution here
        }

        // Otherwise, create new threads for the two recursive calls
        thread([&] {
            self(self, n - 1, id * 2);
        });
        
        thread([&] {
            self(self, n - 2, id * 2 + 1);
        });

        // When both threads complete, combine their results
        answers[id]->set(answers[id * 2]->get() + answers[id * 2 + 1]->get());
        // Debug::printf("*** Set value at n %d, id %d\n", n, id);
    };

    // Start Fibonacci computation in a new thread
    thread([&] {
        fibonacci(fibonacci, N, 1);
    });

    Debug::printf("*** %d-th fibonacci number is: %d\n", N + 1, answers[1]->get());
}