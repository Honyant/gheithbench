
#include "debug.h"
#include "shared.h"
#include "threads.h"
#include "barrier.h"

/*
    This test ensures that you are promoting your WeakPtrs atomically. If you fail this test, 
    take another look at your promote() function and think about where you can get preempted. 

    While this program does test for a very specific race condition, if you have other race
    conditions due to not properly using atomics/locking for certain operations, you will likely 
    still fail this tc. In general, if you are getting errors to do with the heap, it is most likely
    due to a bug with your atomics logic.
*/

#define N 15000 

bool destroyed = false; 

struct Tester {
    int x;
    Tester(int val): x(val) {
        destroyed = false;
    }

    ~Tester() {
        destroyed = true;
        // Debug::printf("Destroyed Tester #%d at address %p\n", x, this);
    }
};

void kernelMain() {
    StrongPtr<Barrier> b{new Barrier(1)}; // yay we can free barriers now
    for (int i = 0; i < N; i++) {
        b->sync(); // syncs with the previous loop iteration
        /* 
        this isn't at the bottom of the loop because I want the StrongPtr 
        to go out of scope and get destroyed before syncing
        */

        b = new Barrier(2);
        StrongPtr<Tester> ptr{new Tester(i)};
        WeakPtr<Tester> weak = ptr;

        thread([weak, b]() mutable {
            {
                auto p = weak.promote();
                /*
                When we promote the above weak pointer, there are three cases:
                    1. The StrongPtr in the main thread has already gone out of scope, so we can't 
                    promote it. (returns nullptr) 

                    2. The StrongPtr in the main thread still exists, so we successfully promote!

                    3. The StrongPtr in the main thread gets destroyed as we are in the process of
                    promoting. 
                */
                if (p != nullptr) {
                    if (destroyed) {
                        Debug::printf("*** FAIL, we have a strong pointer to a deleted object!!! :(\n");
                    }
                    else {
                        // Debug::printf("successful promotion\n");
                    } 
                } 
                else {
                    // Debug::printf("couldn't promote (resource destroyed)\n");
                }
            }
            b->sync();
        });
    }
    b->sync();

    Debug::printf("*** success!\n");
}