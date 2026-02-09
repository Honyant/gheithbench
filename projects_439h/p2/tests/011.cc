#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"

// This test soley focuses on testing your thread implementation 
// (although it uses barriers to sync threads and separate tests)
// It will test for correct behavior for the following properties:
// - Threads share the same heap
// - Each thread has its own stack
// - Threads can sync using barriers
// - Threads can stop and yield
// - Main thread can wait for completion of other threads

#define ThreadCount 5

// Test 1
Atomic<uint32_t> heap_global(0);

// Test 2
Atomic<uint32_t> thread_counter(0);

// Test 3
Atomic<uint32_t> start_counter(0);
Atomic<uint32_t> completion_counter(0);
Atomic<uint32_t> yield_counter(0);

/* Called by one core */
void kernelMain(void) {

    auto testControlBarrier = new Barrier(ThreadCount + 1);

    // Test 1: Thread test
    // Threads share global memory
    // Each thread has its own stack
    // Heap is shared
    Debug::printf("*** starting test 1\n");
    for (uint32_t i = 0; i < ThreadCount; i++) {
        thread([i, &testControlBarrier] { // lambda: copy by value
            uint32_t stack_local = 0;
            stack_local++;
            heap_global.fetch_add(1);
            Debug::printf("*** in thread, stack_local=%u\n", stack_local);
            testControlBarrier->sync();
        });
    }
    testControlBarrier->sync();
    Debug::printf("*** all threads done, heap_global=%u\n", heap_global.get());

    testControlBarrier = new Barrier(ThreadCount + 1);
    Debug::printf("*** starting test 2\n");

    // Test 2: Threads with different workloads
    thread([&testControlBarrier] {
        thread_counter.fetch_add(1);
        testControlBarrier->sync();
    });

    thread([&testControlBarrier] {
        thread_counter.fetch_add(2);
        testControlBarrier->sync();
    });

    thread([&testControlBarrier] {
        thread_counter.fetch_add(3);
        testControlBarrier->sync();
    });

    thread([&testControlBarrier] {
        thread_counter.fetch_add(4);
        testControlBarrier->sync();
    });

    thread([&testControlBarrier] {
        thread_counter.fetch_add(5);
        testControlBarrier->sync();
    });

    // Main thread (long work)
    for (int i = 0; i < 1000000; i++) {
        if (i % 500000 == 0) {
            Debug::printf("*** main thread still running...\n");
        }
    }
    // Wait for threads to complete (likely other threads will wait for main thread)
    testControlBarrier->sync();
    Debug::printf("*** all threads done, thread_counter=%u\n", thread_counter.get());


    Debug::printf("*** starting test 3\n");

    // Test 3: Test thread stop() and yield()
    // cannot use barrier because threads will be stopped,
    // keeping us from syncing and waiting to print on the main thread
    // stop immediately
    thread([] {
        start_counter.fetch_add(1);
        completion_counter.fetch_add(1);
        stop();
        completion_counter.fetch_add(1);
    });

    // does work, yields 2 times, then stops
    thread([] {
        start_counter.fetch_add(1);
        for (int i = 0; i < 1000000; i++) {
            if (i % 500000 == 0) {
                yield();
                yield_counter.fetch_add(1);
            }
        }
        completion_counter.fetch_add(1);
        stop();
        completion_counter.fetch_add(1);
    });

    // does work, yields 2 times, then runs to completion
    thread([] {
        start_counter.fetch_add(1);
        for (int i = 0; i < 1000000; i++) {
            if (i % 500000 == 0) {
                yield();
                yield_counter.fetch_add(1);
            }
        }
        completion_counter.fetch_add(1);
    });

    while (completion_counter.get() < 3) {
        yield();
    }

    Debug::printf("*** all threads done, start_counter=%u, completion_counter=%u, yield_counter=%u\n", start_counter.get(), completion_counter.get(), yield_counter.get());
    

}

