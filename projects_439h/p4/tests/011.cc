#include "threads.h"
#include "shared.h"
#include "atomic.h"

struct Counter {
    Atomic<int> value;
    Counter() : value(0) {}
    ~Counter() {
        Debug::printf("*** Counter destroyed at value: %d\n", value.get());
    }
};

constexpr int NUM_THREADS = 4;
constexpr int ITERATIONS = 1000;

struct TestRunner {
    StrongPtr<Counter> sharedCounter;

    // Is a pointer now
    WeakPtr<Counter>* weakCounters[NUM_THREADS];
    StrongPtr<Counter> strongCounters[NUM_THREADS];
    Atomic<int> threadsCompleted;
    
    TestRunner() : sharedCounter(StrongPtr<Counter>::make()), threadsCompleted(0) {
        for (int i = 0; i < NUM_THREADS; i++) {

            // Will this work for dynamic allocation instead of constructor... 
            weakCounters[i] = new WeakPtr<Counter>(sharedCounter);
            strongCounters[i] = sharedCounter;
        }
    }

    ~TestRunner() {
        for (int i = 0; i < NUM_THREADS; i++) {
            delete weakCounters[i];
        }
    }

    void workerThread(int id) {
        for (int i = 0; i < ITERATIONS; i++) {
            if (i % 2 == 0) {
                // Test strong pointer operations
                StrongPtr<Counter> localCopy = strongCounters[id];
                strongCounters[id] = nullptr;
                strongCounters[id] = localCopy;
                

                // Compilation issue fixed?
                if (!(localCopy == nullptr)) {
                    localCopy->value.fetch_add(1);
                }
            } else {
                // Test weak pointer operations
                StrongPtr<Counter> promoted = weakCounters[id]->promote();

                // Compilation issue fixed?
                if (!(promoted == nullptr)) {
                    promoted->value.fetch_add(1);
                }
            }
        }
        
        threadsCompleted.fetch_add(1);
    }
};

void threadWorker(TestRunner* runner, int id) {
    runner->workerThread(id);
}

void threadSafetyTest() {
    Debug::printf("*** Starting thread safety test\n");
    
    TestRunner runner;
    
    // Start worker threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread([&runner, i] { threadWorker(&runner, i); });
    }
    
    // Wait for all threads to complete
    while (runner.threadsCompleted.get() < NUM_THREADS) {
        yield();
    }
    
    // Check final state
    Debug::printf("*** Final counter value: %d\n", runner.sharedCounter->value.get());
    
    // Test that all weak pointers can still be promoted
    for (int i = 0; i < NUM_THREADS; i++) {
        auto promoted = runner.weakCounters[i]->promote();
        Debug::printf("*** Weak pointer %d promotion %s\n", i, 
            promoted == nullptr ? "failed" : "successful");
    }
    
    // Clear all strong pointers
    for (int i = 0; i < NUM_THREADS; i++) {
        runner.strongCounters[i] = nullptr;
    }
    runner.sharedCounter = nullptr;
    
    // Try to promote weak pointers after all strong pointers are gone, should fail
    for (int i = 0; i < NUM_THREADS; i++) {
        auto promoted = runner.weakCounters[i]->promote();
        Debug::printf("*** Post-cleanup weak pointer %d promotion %s\n", i, 
            promoted == nullptr ? "failed" : "successful");
    }
}

void kernelMain(void) {
    Debug::printf("*** Starting thread safety tests\n");
    threadSafetyTest();
    Debug::printf("*** Thread safety tests completed\n");
}