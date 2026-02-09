#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"
#include "promise.h"

constexpr uint32_t N = 15;
constexpr uint32_t expected = 7746000;

uint32_t magic_number = 0;


/* Called by one core */
void kernelMain(void) {
    Debug::printf("*** Test Start ***\n");

    //Check that basic barrier and promise implementation works

    Barrier *b = new Barrier(N+1);

    //Creating array to ensure an order that threads run in
    Promise<bool> *promiseOrder = new Promise<bool>[N];
    promiseOrder[0].set(true);

    //Goal is to calculate a number with alternating operations and numbers 1-15
    //Output is the result of (...(((((0 * 1) + 2 ) * 3) + 4) * 5)... * 15)

    for (uint32_t i=1; i<=N; i++) {
        thread([i, b, &promiseOrder] {
            promiseOrder[i - 1].get();
            if (i % 2 == 0) {
                magic_number += i;
            } else {
                magic_number *= i;
            }
            if (i != N) {
                promiseOrder[i].set(true);
            }
            b->sync();
        });
    }

    b->sync();

    if (magic_number == expected) {
        Debug::printf("*** Good job, your promises and barriers promised and barred\n");
    } else {
        Debug::printf("*** %d != %d\n", magic_number, expected);
    }

    //Testing yield with barrier with a wait queue (ideally)


    Barrier* barrier = new Barrier(101);

    Atomic<bool> done = false;
    for (int i = 0; i < 100; i++) {
        thread([barrier, &done] {
            barrier->sync();
            done.set(true);
        });
    }

    for (int i = 0; i < 5; i++) {
        thread([&done] {
            while(!done.get()) {
                yield();
            }
        });
    }

    barrier->sync();

    Debug::printf("*** Congrats, you have implemented basic yield and barriers sufficiently well for this test\n");
    Debug::printf("*** Test End ***\n");

}