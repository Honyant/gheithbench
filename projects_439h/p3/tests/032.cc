#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "promise.h"
#include "barrier.h"
#include "bb.h"


/*
tests basic functionality of barriers, promise, thread, bounded buffers, sleep, preemption
testcase is modular so you can split up the code by the comments if needed
*/

Atomic<uint32_t> thread1counter{1};
Atomic<uint32_t> thread2counter{0};
Atomic<uint32_t> counter{0};
Atomic<uint32_t> bbCounter{0};
int arr[5] = {49, 64, 81, 100, 121};


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

    // basic preemption check
    for (int i = 0; i < 5; i++) {
        thread([] {
            Debug::printf("*** preemption\n");
            counter.fetch_add(1);
        });
    }
    
    while (counter.get() != 5);

    // bounded buffer basic test
    auto *buffer1 = new BB<int>(3);
    buffer1->put(arr[0]);
    buffer1->put(arr[1]);
    buffer1->put(arr[2]);
    Debug::printf("*** buffer = %d\n",buffer1->get());
    Debug::printf("*** buffer = %d\n",buffer1->get());
    Debug::printf("*** buffer = %d\n",buffer1->get());

    // bounded buffer blocking
    auto *buffer2 = new BB<int>(1);

    // get block
    thread([buffer2] {
        Debug::printf("*** adding to buffer\n");
        buffer2->put(arr[3]);
    });

    Debug::printf("*** got %d from buffer\n",buffer2->get());

    buffer2->put(arr[0]);

    // put block
    thread([buffer2] {
        Debug::printf("*** removing from buffer\n");
        buffer2->get();
    });

    buffer2->put(arr[4]);
    Debug::printf("*** got %d from buffer\n",buffer2->get());


    int coreCounter = kConfig.totalProcs; 
    auto** buffers = new BB<int>*[coreCounter];

    for (int i = 0; i < coreCounter; ++i) {
        buffers[i] = new BB<int>(50);
    }

    for (int i = 0; i < 50; ++i) {
        thread([buffers, coreCounter] {
            buffers[bbCounter.get() % coreCounter]->put(arr[4]);
            bbCounter.add_fetch(1);
        });
    }

    for (int i = 0; i < coreCounter; ++i) {
        ASSERT(buffers[i]->get() == arr[4]);
    }

    // Print completion message
    Debug::printf("*** bounded buffer stress test check done\n");


    // basic sleep check
    for (uint32_t i = 0; i < 500; i++) {
        thread([] {
            sleep(1);
        });
    }
    Debug::printf("*** honk shoo mimimimimi\n");
    Debug::printf("*** all done\n");
}