#include "threads.h"
#include "debug.h"
#include "promise.h"
#include "barrier.h"
#include "bb.h"

// This test case builds on my test case from p2

// Shared resources
Atomic<int> counter = 0;


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



    // p3 specific stuff

    BB<int>* bb = new BB<int>(1);
    Barrier* barrier4 = new Barrier(21);
    
    //try passing many puts through a buffer too small to accept everything at once
    for(int i = 0; i < 10; i++){
        thread([barrier4, bb] {
            for(int i = 0; i < 10; i++){
                int x = 0;
                bb->put(x);
            }
            barrier4->sync();
        });
    }

    for(int i = 0; i < 10; i++){
        thread([barrier4, bb] {
            for(int i = 0; i < 10; i++){
                int value = bb->get();
                ASSERT(value == 0);
            }
            barrier4->sync();
        });
    }

    barrier4->sync();

    // test that sleeping threads get woken in the right order
    Barrier* barrier5 = new Barrier(21);

    for (uint32_t i = 0; i < 10; i++) {
        thread([&barrier5] {
            sleep(1);
            Debug::printf("*** Should print in first block\n");
            barrier5->sync();
        });
    }

    for (uint32_t i = 0; i < 10; i++) {
        thread([&barrier5] {
            sleep(2);
            Debug::printf("*** Should print in second block\n");
            barrier5->sync();
        });
    }

    barrier5->sync();

    Debug::printf("*** Done\n");

}
