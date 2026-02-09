#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"
#include "promise.h"

constexpr uint32_t N = 4;

void kernelMain(void) {
    Debug::printf("*** beginning\n");

    // the barriers help synchronize the order of the print statements to (hopefully) prevent race conditions
    auto b1 = new Barrier(N + 1);
    auto b2 = new Barrier(N + 1);
    auto b3 = new Barrier(N + 1);
    auto b4 = new Barrier(2);
    auto b5 = new Barrier(2);
    auto b6 = new Barrier(2);

    // basic barriers test
    // b2 helps prevent race conditions

    for (uint32_t i=1; i<=N; i++) {
        thread([b1, b2, b3] {
            Debug::printf("*** hello\n");
            b1->sync();
            b2->sync();
            b3->sync();
        });
    }
    b1->sync();
    b2->sync();

    Debug::printf("*** This should come after\n");
    b3->sync();

    // promises test – set and get
    auto promise1 = new Promise<int>();
    thread([promise1] {
        promise1->set(1000);
    });

    thread([promise1, b4] {
        Debug::printf("*** promise 1 = %d\n",promise1->get());
        b4->sync();
    });

    b4->sync();

    // promises test – chaining promises together
    auto promise2 = new Promise<int>();
    thread([promise1, promise2] {
        int value = promise1->get();
        promise2->set(value * 2);  // Chain promise2 to depend on promise1
    });

    thread([promise2, b5] {
        Debug::printf("*** promise 2 = %d\n",promise2->get());
        b5->sync();
    });

    b5->sync();

    // conditional promises test
    auto promise3 = new Promise<int>();
    auto promise4 = new Promise<int>();

    thread([promise3, promise4] {
        int value = promise3->get();
        if (value > 500) {
            promise4->set(value * 2);
        } else {
            promise4->set(value + 100);
        }
    });

    thread([promise4, b6] {
        Debug::printf("*** Conditional promise 4 = %d\n", promise4->get());
        b6->sync();
    });

    promise3->set(600);
    b6->sync();

    
}
