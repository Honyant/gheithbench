#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "promise.h"
#include "barrier.h"

/*
tests basic functionality for barriers, threads, and promises
*/
Atomic<uint32_t> thread1counter{1};
Atomic<uint32_t> thread2counter{0};


void kernelMain(void) {
    // tests promise set then get
    auto promise1 = new Promise<int>();
    Barrier* barrier1 = new Barrier(2);

    thread([promise1, barrier1] {
        promise1->set(25);
        barrier1->sync();
    });

    barrier1->sync();
    Debug::printf("*** promise = %d\n",promise1->get());

    // tests promise get then set
    auto promise2 = new Promise<int>();

    thread([promise2] {
        promise2->set(36);
    });

    Debug::printf("*** promise = %d\n",promise2->get());

    // tests switching control between threads with promises and using multiple barriers
    Barrier* barrier2 = new Barrier(2);
    Barrier* barrier3 = new Barrier(2);
    auto promise3 = new Promise<int>();
    auto promise4 = new Promise<int>();

    thread([barrier2, promise3, barrier3, promise4] {
        promise4->get();
        Debug::printf("*** b\n");
        barrier2->sync();
        promise3->get();
        Debug::printf("*** d\n");
        barrier3->sync();
    });
    Debug::printf("*** a\n");
    promise4->set(64);
    barrier2->sync();
    Debug::printf("*** c\n");
    promise3->set(49);
    barrier3->sync();

    // tests multiple threads and yielding

    Barrier* barrier4 = new Barrier(3);

    thread([barrier4] {
        while (thread1counter.get() % 2 == 1) yield();
        Debug::printf("*** two\n");
        thread2counter.fetch_add(1);
        barrier4->sync();
    });

    thread([barrier4] {
        Debug::printf("*** one\n");
        thread1counter.fetch_add(1);
        while(thread2counter.get() % 2 == 0) yield();
        Debug::printf("*** three\n");
        barrier4->sync();
    });

    barrier4->sync();
    Debug::printf("*** all done\n");

}

