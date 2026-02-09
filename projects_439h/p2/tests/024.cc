#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "promise.h"
#include "barrier.h"

const uint32_t N = 10;

void recurse(Atomic<uint32_t> &count, Barrier *barrier) {
    count.fetch_add(1);

    if (count.get() < N) {
        thread([&count, barrier] {
            recurse(count, barrier);
        });
    }

    barrier->sync();
}

void kernelMain(void) {
    // Test 1: Summing with multiple threads
    Debug::printf("*** TEST 1\n");
    auto barrier1 = new Barrier(N + 1);
    Atomic<uint32_t> count1 = 0;

    for (uint32_t i = 1; i <= N; i += 2) {
        thread([i, &count1, barrier1] {
            count1.fetch_add(i);
            barrier1->sync();
        });
        
        thread([i, &count1, barrier1] {
            count1.fetch_add(i + 1);
            barrier1->sync();
        });
    }

    barrier1->sync();
    Debug::printf("*** Result: %d\n", count1.get());
    Debug::printf("***\n");

    // Test 2: Nested thread recursion
    Debug::printf("*** TEST 2\n");
    auto barrier2 = new Barrier(N + 1);
    Atomic<uint32_t> count2 = 0;
    
    thread([&count2, barrier2] {
        recurse(count2, barrier2);
    });

    barrier2->sync();
    Debug::printf("*** Result: %d\n", count2);
    Debug::printf("***\n");

    // Test 3: Nested promises
    Debug::printf("*** TEST 3\n");
    auto promise1 = new Promise<Promise<int>*>();
    auto promise2 = new Promise<int>();

    thread([promise1, promise2] {
        promise1->set(promise2);
    });

    thread([promise2] {
        promise2->set(20);
    });

    Debug::printf("*** Result: %d\n", promise1->get()->get());
    Debug::printf("***\n");

    // Test 4: Multiple yields per thread
    Debug::printf("*** TEST 4\n");
    auto barrier3 = new Barrier(N + 1);
    Atomic<uint32_t> count3 = 0;

    for (uint32_t i = 0; i < N; i++) {
        thread([&count3, barrier3] {
            count3.fetch_add(1);
            yield();
            count3.fetch_add(2);
            yield();
            count3.fetch_add(3);
            yield();
            count3.fetch_add(4);
            yield();
            barrier3->sync();
        });
    }

    barrier3->sync();
    Debug::printf("*** Result: %d\n", count3);
}
