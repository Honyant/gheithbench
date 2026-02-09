#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "promise.h"
# include "barrier.h"

/**
 * This test case creates a bunch of Promises in a loop to try to look
 * for race conditions in your implementation. If you're deadlocking,
 * there is likely a race condition related to your waiting queue for 
 * Promise and/or Barrier.
*/

constexpr int n = 100;
constexpr int m = 4;
Atomic<uint32_t> count{0};

void kernelMain(void) {
    Debug::printf("*** hello! :D\n");
    
    Barrier* barrier = new Barrier(m * n + 1);
    for(int i = 0; i < n; i++) {
        Promise<int>* promise = new Promise<int>();
        for(int j = 0; j < m/2; j++) {
            thread([promise, i, barrier] {
                count.fetch_add(promise->get());
                barrier->sync();
            });
        }

        thread([promise, i] {
            promise->set(i+1);
        });
        
        for(int j = 0; j < m/2; j++) {
            thread([promise, i, barrier] {
                count.fetch_add(promise->get());
                barrier->sync();
            });
        }
    }

    barrier->sync();
    Debug::printf("*** bye! %d\n", count.get());
}