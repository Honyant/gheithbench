#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"
#include "atomic.h"
#include "blocking_lock.h"
#include "bb.h"

const int CORES = kConfig.totalProcs;

void kernelMain(void) {
    // Test 1: Preemption
    Debug::printf("*** TEST 1\n");
    Atomic<int> count1 = 0;

    for (int i = 0; i < CORES; i++) {
        thread([&count1] {
            count1.fetch_add(1);
            while (count1.get() < CORES);
        });
    } 

    while (count1.get() < CORES);
    Debug::printf("*** All threads ran\n");
    Debug::printf("***\n");


    // Test 2: Blocking lock shared variables
    Debug::printf("*** TEST 2\n");
    const int N2 = 200;
    Barrier *b2 = new Barrier(N2 + 1);
    BlockingLock *lock2 = new BlockingLock();
    int count2 = 0;
    
    for (int i = 0; i < N2; i++) {
        thread([&count2, lock2, b2] {
            lock2->lock();
            count2++;
            lock2->unlock();
            b2->sync();
        });
    }

    b2->sync();
    Debug::printf("*** Count: %d\n", count2);
    Debug::printf("***\n");


    // Test 3: Bounded buffer ordering
    Debug::printf("*** TEST 3\n");
    BB<int> *bb3 = new BB<int>(10);
    Barrier *b3 = new Barrier(2);

    thread([bb3, b3] {
        for (int i = 1; i <= 10; i++) {
            Debug::printf("*** %d\n", bb3->get());
        }

        b3->sync();
    });
   
    for (int i = 1; i <= 10; i++) {
        bb3->put(i);
    }

    b3->sync();
    Debug::printf("***\n");


    // Test 4: Sleep blocking
    Debug::printf("*** TEST 4\n");
    const int N4 = 50;
    Atomic<int> count4 = 0;

    for (int i = 0; i < N4; i++) {
        thread([&count4] {
            sleep(1);
            count4.fetch_add(1);
        });
    } 

    while (count4.get() < N4);
    Debug::printf("*** All threads woke up\n");
}

