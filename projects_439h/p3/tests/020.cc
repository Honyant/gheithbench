#include "debug.h"
#include "threads.h"
#include "bb.h"
#include "barrier.h"
#include "promise.h"

constexpr int N = 100;

/**
 * This is a test of bb. It tests the waiting queue piece of the bb, 
 * and combines it with other synchronization primitives. 
 */
void kernelMain(void) {
    bool wasDisabled = Interrupts::disable();
    Debug::printf("*** beginning\n");
   
    // basic putting and getting of bb
    auto b1 = new Barrier(2);
    auto bb1 = new BB<int>(2);

    thread([bb1, b1] {
        int a = 1;
        bb1->put(a);
        int b = 2;
        bb1->put(b);
        b1->sync();
    });
    
    int first = bb1->get();
    ASSERT(first == 1);
    int second = bb1->get();
    ASSERT(second == 2);
    Debug::printf("*** Basic getting and setting bb test passed\n");
    b1->sync();

    // combining promises and bb
    auto promise2 = new Promise<int>();
    auto bb2 = new BB<int>(2);
    auto b2 = new Barrier(2);
    auto b3 = new Barrier(2);
    auto b4 = new Barrier(2);

    thread([promise2, b2] {
        Debug::printf("*** promise 2 is %d\n", promise2->get());
        b2->sync();
    });

    thread([promise2, bb2, b3] {
        int a = 1;
        bb2->put(a);
        int b = 2;
        bb2->put(b);
        int c = 3;
        bb2->put(c); // this now should cause the thread to block
        promise2->set(1000);
        b3->sync();
    });

    // int valFromBB2 = bb2->get(); // this should cause the earlier thread to stop blocking and the print statement should print
    // ASSERT(valFromBB2 == 1);

    thread([bb2, b4] {
        int valFromBB2 = bb2->get();  // Should unblock the producer thread
        ASSERT(valFromBB2 == 1);
        int valFromBB2_2 = bb2->get();
        ASSERT(valFromBB2_2 == 2);
        b4->sync(); 
    });

    b2->sync();

    b3->sync();
    Debug::printf("*** after b3\n");
    b4->sync();
    Debug::printf("*** All threads synchronized and completed\n");


    Interrupts::restore(wasDisabled);

}