#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "promise.h"
#include "barrier.h"
#include "bb.h"

/*Functional test*/
Atomic<uint32_t> num{0};

void kernelMain(void) {
    // bounded buffer (fifo, size) and promise test
    auto bb1 = new BB<int>(2);
    auto p1 = new Promise<int>();
    auto p2 = new Promise<int>();
    auto p3 = new Promise<int>();
    auto b1 = new Barrier(5);
    thread([p1, b1] {
        p1->set(16);
        b1->sync();
    });
    thread([p2, b1] {
        p2->set(14);
        b1->sync();
    });
    thread([p3, b1] {
        p3->set(15);
        b1->sync();
    });
    thread([bb1, b1, p1] {
        int x = p1->get();
        bb1->put(x);
        b1->sync();
    });
    b1->sync();
    auto b2 = new Barrier(2);
    thread([bb1, b2, p2] {
        int x = p2->get();
        bb1->put(x);
        b2->sync();
    });
    b2->sync();
    auto b3 = new Barrier(2);
    thread([bb1, b3, p3] {
        int x = p3->get();
        bb1->put(x); // block since full
        b3->sync();
    });
    // num should be 16
    num.add_fetch(bb1->get()); // unblocks
    Debug::printf("*** num = %d\n", num);
    b3->sync();

    // num should be 30
    num.add_fetch(bb1->get());
    Debug::printf("*** num = %d\n", num);
    // num should be 45
    num.add_fetch(bb1->get());
    Debug::printf("*** num = %d\n", num);
    
}

