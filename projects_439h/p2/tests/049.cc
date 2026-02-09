#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "promise.h"
#include "barrier.h"

/*Functional test*/
Atomic<uint32_t> num{0};

void kernelMain(void) {
    // Simple barrier test
    auto b1 = new Barrier(5);
    
    for (int i=0; i<4; i++) {
        thread([b1] {
            num.fetch_add(1);
            b1->sync();
        });
    }
    // num should be 4
    b1->sync();
    Debug::printf("*** num = %d\n", num);
    
    // Simple promise test
    auto p1 = new Promise<int>();
    auto b2 = new Barrier(2);
    thread([p1, b2] {
        p1->set(12);
        b2->sync();
    });

    // num should be 16
    b2->sync();
    num.add_fetch(p1->get());
    Debug::printf("*** num = %d\n", num);

    // Setting data
    auto p2 = new Promise<int>();
    auto p3 = new Promise<int>();
    auto p4 = new Promise<int>();
    auto p5 = new Promise<int>();
    auto p6 = new Promise<int>();
    auto b3 = new Barrier(6);
    thread([p2, b3] {
        p2->set(2);
        b3->sync();
    });
    thread([p3, b3] {
        p3->set(3);
        b3->sync();
    });
    thread([p4, b3] {
        p4->set(4);
        b3->sync();
    });
    thread([p5, b3] {
        p5->set(5);
        b3->sync();
    });
    thread([p6, b3] {
        p6->set(6);
        b3->sync();
    });
    b3->sync();

    // Calc
    auto p7 = new Promise<int>();
    auto b4 = new Barrier(2);
    thread([p2, p3, p4, p5, p6, p7, b4] {
        p7->set(p2->get() + p3->get() + p4->get() + p5->get() + p6->get());
        b4->sync();
    });
    b4->sync();
    // Printing output
    // should be 20
    Debug::printf("*** promise = %d\n", p7->get());    
}

