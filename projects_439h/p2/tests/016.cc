#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "promise.h"

/**
 * This test case stresses your promise implementation.
 * Specifically, it queues up a large number of threads 
 * into your promise queue before releasing them.
 * This should help expose any race conditions and 
 * performance issues with your promises.
 * For debugging, think back to what Gheith discussed 
 * on the 9th.
 */

void kernelMain(void) {
    Debug::printf("*** Test started.\n");

    Atomic<int> promise_ready_count{0};
    Atomic<int> promise_done_count{0};
    auto promise = new Promise<int>();
    int promise_value = 123;
    int numPromiseThreads = 200;

    for(int i = 1 ; i <= numPromiseThreads; i++) {
      thread([promise_value, i, promise, &promise_ready_count, &promise_done_count] {
        // Note: it is possible for the ready count to increment
        // prior to the promise being added. Due to its low probability
        // and because numPromiseThreads is sufficiently high, this is OK
        promise_ready_count.add_fetch(1);
        int val = promise->get();
        if(val != promise_value) {
          Debug::printf("*** Received incorrect promise value on iteration %d\n", i);
          Debug::shutdown();  
        }
        promise_done_count.add_fetch(1);
      });
    }
   
    while(promise_ready_count.get() != numPromiseThreads) yield();
    Debug::printf("*** All promises ready.\n");
    
    promise->set(promise_value);
    while(promise_done_count.get() != numPromiseThreads) yield();

    Debug::printf("*** Promise test cleared.\n");
}

