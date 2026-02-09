#include "debug.h"
#include "threads.h"
#include "promise.h"
#include "barrier.h"
#include "atomic.h"

// Test1: Computes large sum with several long running threads (sanity check)

// Test2: Computes large sum with lots of Short Threads as a Stress Test (stresses yield and checks for memory)

// Test3: Large Promise/Barrier Queue

void kernelMain(void) {
    
    // Test1
    Debug::printf("*** Test 1\n");
    int n_threads = 200;
    Atomic<int32_t> counter{0};
    Atomic<int32_t> finished{0};
    Barrier *b = new Barrier{n_threads + 1};

    for (int i = 0; i < n_threads; i ++) {
        thread([&counter, &finished, b] {
            for (int i = 0; i < 10000; i ++) {
                counter.fetch_add((int32_t) 1);
                if (i % 1000 == 0) {
                    yield();
                }
            }
            finished.fetch_add(1);
            b->sync();
            stop();
        });
    }

    yield();
    b->sync();
    Debug::printf("*** Finished threads: %d\n", finished.get());
    Debug::printf("*** Counter is at %d\n", counter.get());

    // Test 2
    Debug::printf("*** Test 2\n");

    int n_threads2 = 300;
    Atomic<int32_t> finished2{0};
    Barrier *b2 = new Barrier{n_threads2 + 1};

    for (int i = 0; i < n_threads2; i ++) {
        thread([&finished2, b2] {
            finished2.fetch_add(1);
            b2->sync();
            stop();
        });
    }

    yield();
    b2->sync();
    Debug::printf("*** Finished threads %d\n", finished2.get());

    // Test 3
    Debug::printf("*** Test 3\n");

    int n_threads3 = 200;
    Atomic<int> counter2{0};
    Atomic<int> sum{0};
    Promise<int> *answer = new Promise<int>{};
    Barrier *b3 = new Barrier{n_threads3 + 1};
    for (int i = 0; i < n_threads3; i ++) {
        thread([answer, b3, &sum, &counter2] {
            counter2.add_fetch(1);
            sum.add_fetch(answer->get());
            b3->sync();
            stop();
        });
    }
    while (counter2.get() < n_threads3) {
        yield();
    }
    answer->set(3);
    counter2.set(0);
    b3->sync();

    Debug::printf("*** Sum is %d\n", sum);
    for (int i = 0; i < n_threads + n_threads2 + n_threads3 + 100; i ++) {
        yield();
    }
}