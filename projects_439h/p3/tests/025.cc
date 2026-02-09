// Not a sleep test
// Tests Bounded Buffers, Promises, Barriers, basic pre-emption, interrupt state conservation


#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"
#include "promise.h"
#include "bb.h"

constexpr uint32_t N = 16;
uint32_t one = 1;

void kernelMain(void) {
    Debug::printf("*** Test Start ***\n");

    //There will be a couple of asserts sprinkled throughout to ensure you conserve the interrupt state of the thread
    ASSERT(!Interrupts::isDisabled());

    //Check that basic barrier and promise implementation works with bounded buffer and simple FIFO
    Barrier *b = new Barrier(N+1);
    BB<uint32_t> *boundedBuffer = new BB<uint32_t>(N);
    boundedBuffer->put(one);

    //Creating array to ensure an order that threads run in (testing order within buffer capacity)
    Promise<bool> *promiseOrder = new Promise<bool>[N];
    promiseOrder[0].set(true);
    
    // Generates 16 16s in buffer (requires FIFO implementation)
    for (uint32_t i=1; i<=N; i++) {
        thread([i, b, &promiseOrder, boundedBuffer] {
            promiseOrder[i - 1].get();
            ASSERT(!Interrupts::isDisabled());
            uint32_t magic = boundedBuffer->get();
            uint32_t magic_transform = magic * 2;
            boundedBuffer->put(magic_transform);
            if (i != N) {
                boundedBuffer->put(magic_transform);
                promiseOrder[i].set(true);
            }
            ASSERT(!Interrupts::isDisabled());
            b->sync();
        });
    }
    b->sync();

    //Verify order of buffer
    uint32_t matchNum = 16;
    for (uint32_t i = 1; i < N; i++) {
        uint32_t output = boundedBuffer->get();
        if (output != matchNum) {
            Debug::printf("*** Buffer position %d had incorrect value. Expected: %d, Got: %d\n", i, matchNum, output);
        }
    }
    ASSERT(!Interrupts::isDisabled());
    Debug::printf("*** Test 1 complete\n");
    
    //Test Basic pre-emption and interrupt state saving
    bool wasDisabled = Interrupts::disable();

    //Mark when arriving to infinite loop and when we pre-empt
    bool* arrival_array = new bool[kConfig.totalProcs];
    bool* yielded_array = new bool[kConfig.totalProcs];
    for (uint32_t i = 0; i < kConfig.totalProcs; i++) {
        arrival_array[i] = false;
        yielded_array[i] = false;
    }
    
    for (uint32_t i = 0; i < kConfig.totalProcs; i++) {
        ASSERT(Interrupts::isDisabled());
        thread([i, &arrival_array, &yielded_array] {
            arrival_array[i] = true;
            while(true) {
                if (yielded_array[i]) {
                    stop();
                }
            }
        });
        ASSERT(Interrupts::isDisabled());
    }

    for (uint32_t i = 0; i < kConfig.totalProcs; i++) {
        ASSERT(Interrupts::isDisabled());
        thread([i, &arrival_array, &yielded_array] {
            while (!arrival_array[i]);
            yielded_array[i] = true;
        });
        ASSERT(Interrupts::isDisabled());
    }
    Interrupts::restore(wasDisabled);


    Debug::printf("*** Test 2 complete\n");

    //Test simple Bounded Buffer order among threads beyond fill capacity
    BB<uint32_t> *boundedBuffer1 = new BB<uint32_t>(N);

    for (uint32_t core = 0; core < kConfig.totalProcs; core++) {
        thread([boundedBuffer1] {
            for (uint32_t i = 1; i <= N; i++) {
                boundedBuffer1->put(i);
            }
            for (uint32_t i = 1; i <= N; i++) {
                uint32_t x = boundedBuffer1->get();
                if (x != i) {
                    Debug::printf("*** FIFO order not conserved beyond buffer capacity");
                }
            }
        });
    }

    
    Debug::printf("*** Test 3 complete\n");
    
    //Final sneaky assert
    ASSERT(!Interrupts::isDisabled());

    Debug::printf("*** Congrats, you have implemented bounded buffers, basic pre-emption, and maintained state sufficiently well for this test\n");
    Debug::printf("*** Test End ***\n");

}