#include "threads.h"
#include "debug.h"
#include "promise.h"
#include "barrier.h"

// Shared resources
Atomic<int> counter = 0;

void recursive_function(int iteration) {
    Debug::printf("*** Iteration %d\n", iteration);

    if (iteration >= 500) {
        Debug::printf("*** Reached max iterations, stopping recursion\n");
        Debug::shutdown();
    }

    thread([iteration] {
        recursive_function(iteration + 1);
    });

    while(true) yield();
}


void kernelMain(void) {

    //basic test with multiple barriers and promises
    Promise<int>* promise1 = new Promise<int>();
    Promise<int>* promise2 = new Promise<int>();
    Promise<int>* promise3 = new Promise<int>();

    Barrier* barrier1 = new Barrier(3);
    Barrier* barrier2 = new Barrier(3);
    
    thread([promise1, barrier1, barrier2] {
        barrier1->sync();
        counter.fetch_add(1);
        barrier2->sync();
        promise1->set(counter);
    });

    thread([promise2, barrier1, barrier2] {
        barrier1->sync();
        counter.fetch_add(2);
        barrier2->sync();
        promise2->set(counter);
    });

    thread([promise3, barrier1, barrier2] {
        barrier1->sync();
        counter.fetch_add(3);
        barrier2->sync();
        promise3->set(counter);
    });

    yield();
    yield();

    int value1 = promise1->get();
    int value2 = promise2->get();
    int value3 = promise3->get();

    Debug::printf("*** Promise 1 value: %d\n", value1);
    Debug::printf("*** Promise 2 value: %d\n", value2);
    Debug::printf("*** Promise 3 value: %d\n", value3);

    //stress test that creates a lot of simple threads
    Debug::printf("*** Starting stress test\n");
    recursive_function(1);
}
