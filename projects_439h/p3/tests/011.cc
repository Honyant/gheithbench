#include "debug.h"
#include "threads.h"
#include "barrier.h"
#include "bb.h"


// This is a bounded buffer test case that aims to check a variety of common functionality.
// I have also sprinkled asserts that the interrupts are still enabled throughout the test case, because it is VERY easy
// to accidentally be disabling interrupts without realizing it.
// This test cases uses new, so if you don't have a working blocking lock, you may want to swap the one on the heap out for a spin lock for now.

constexpr uint32_t bufferSize = 10;

Atomic<uint32_t> count = 0;

/* Called by one core */
void kernelMain(void) {

    ASSERT(Interrupts::isDisabled() == false);
    // Part 1: Single put and get, no queueing necessary.
    auto boundedBuffer1 = new BB<int>(bufferSize);
    auto barrier1 = new Barrier(2);
    Debug::printf("*** Starting Part 1\n");
    thread([barrier1, boundedBuffer1] {
        //int gotValue = boundedBuffer1->get();
        //if(gotValue != 1){
        //    Debug::printf("*** Part 1 Error: Wrong value recieved\n");
        //}
        barrier1->sync();
    });

    //int value = 1;
    //boundedBuffer1->put(value);
    //ASSERT(Interrupts::isDisabled() == false);
    barrier1->sync();
    ASSERT(Interrupts::isDisabled() == false);

    Debug::printf("*** Part 1 Complete\n");
    ASSERT(Interrupts::isDisabled() == false);

    // Part 2: Multiple puts and gets, no waiting to put necessary.
    auto boundedBuffer2 = new BB<int>(bufferSize);
    auto barrier2 = new Barrier(2);
    int *  values2 = new int[5];
    values2[0] = 1;
    values2[1] = 22;
    values2[2] = 333;
    values2[3] = 4444;
    values2[4] = 55555;
    
    Debug::printf("*** Starting Part 2\n");
    ASSERT(Interrupts::isDisabled() == false);
    thread([barrier2, boundedBuffer2, values2] {
        ASSERT(Interrupts::isDisabled() == false);
        for(int i = 0; i < 5; i++){
            int gotValue = boundedBuffer2->get();
            if(values2[i] != gotValue){
                Debug::printf("*** Part 2 Error: Wrong value %d recieved on iteration %d. expected %d\n", gotValue, i, values2[i]);
            }
        }
        barrier2->sync();
    });

    for(int i = 0; i < 5; i++){
        boundedBuffer2->put(values2[i]);
        ASSERT(Interrupts::isDisabled() == false);
    }
    barrier2->sync();
    ASSERT(Interrupts::isDisabled() == false);

    Debug::printf("*** Part 2 Complete\n");
    ASSERT(Interrupts::isDisabled() == false);

    // Part 3: Multiple puts and gets, waiting to put will likely be necessary.
    auto boundedBuffer3 = new BB<int>(bufferSize);
    auto barrier3 = new Barrier(2);
    int *  values3 = new int[100];
    for(int i = 0; i < 100; i++){
        values3[i] = i;
        ASSERT(Interrupts::isDisabled() == false);
    }
    
    Debug::printf("*** Starting Part 3\n");
    ASSERT(Interrupts::isDisabled() == false);
    thread([barrier3, boundedBuffer3, values3] {
        for(int i = 0; i < 100; i++){
            int gotValue = boundedBuffer3->get();
            if(values3[i] != gotValue){
                Debug::printf("*** Part 3 Error: Wrong value %d recieved on iteration %d. expected %d\n", gotValue, i, values3[i]);
            }
        }
        barrier3->sync();
    });

    for(int i = 0; i < 100; i++){
        boundedBuffer3->put(values3[i]);
    }
    barrier3->sync();

    Debug::printf("*** Part 3 Complete\n");

    // Part 4: Multiple putters and getters at the same time
    auto boundedBuffer4 = new BB<int>(bufferSize);
    auto barrier4 = new Barrier(9);
    int value4 = 4;
    
    Debug::printf("*** Starting Part 4\n");
    
    // Creating 3 putter threads
    for(int i = 0; i < 4; i++){
        ASSERT(Interrupts::isDisabled() == false);
        thread([barrier4, boundedBuffer4, value4] {
            ASSERT(Interrupts::isDisabled() == false);
            for(int i = 0; i < 10; i++){
                int putValue4 = value4;
                boundedBuffer4->put(putValue4);
            }
            //Debug::printf("Putter thread done!\n");
            ASSERT(Interrupts::isDisabled() == false);
            barrier4->sync();
        });
    }

    // Creating 3 getter threads
    for(int i = 0; i < 4; i++){
        ASSERT(Interrupts::isDisabled() == false);
        thread([barrier4, boundedBuffer4, value4] {
            ASSERT(Interrupts::isDisabled() == false);
            for(int i = 0; i < 10; i++){
                int gotValue = boundedBuffer4->get();
                if(gotValue != 4){
                    Debug::printf("*** Part 4 Error: Wrong value %d recieved on iteration %d. expected %d\n", gotValue, i, 4);
                }
            }
            //Debug::printf("Getter thread done!\n");
            ASSERT(Interrupts::isDisabled() == false);
            barrier4->sync();
        });
    }

    // Sync to ensure all puts and gets are completed
    //Debug::printf("Calling sync!\n");
    ASSERT(Interrupts::isDisabled() == false);
    barrier4->sync();

    Debug::printf("*** Part 4 Complete\n");

}