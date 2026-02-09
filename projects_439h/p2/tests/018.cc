#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "promise.h"
#include "barrier.h"

Atomic<bool> flag1 = false;
Atomic<bool> promiseFulfilled2 = false;

constexpr uint32_t N = 10;

/* Called by one core */
void kernelMain(void) {
    // test Promise
    auto helloKitty = new Promise<int>();
    auto cinnamoroll = new Promise<int>();
    
    thread([helloKitty, cinnamoroll] {
        while (!flag1.get()) {
            yield();
        }
        Debug::printf("*** who is your fav character?\n"); // this should print second since the promises haven't been set yet
        helloKitty->set(2);
        cinnamoroll->set(1);
    });

    Debug::printf("*** WELCOME TO SANRIO WORLD\n");
    flag1.set(true); // ensure that the welcome message gets printed first

    Debug::printf("*** %d hello kitties\n",helloKitty->get());
    Debug::printf("*** %d cinnamoroll\n",cinnamoroll->get());

    // test Barrier and array of Promises

    Promise<int>* promiseArr = new Promise<int>[N];
    auto b = new Barrier(N+1);
    for (uint32_t i = 0; i < N; i++) {
        thread([b, i, promiseArr] {
            promiseArr[i].set(i + 1);
            b->sync();
        });
    }

    b->sync();
    Debug::printf("*** %d kuromi\n",cinnamoroll->get()); // testing multiple gets
    Debug::printf("***");
    for (uint32_t i = 0; i < N; i++) {
        Debug::printf(" %d", promiseArr[i].get());
    }
    Debug::printf("\n");
}