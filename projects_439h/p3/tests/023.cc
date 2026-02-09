#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "bb.h"
#include "barrier.h"

Atomic<uint32_t> count = 0;

void kernelMain(void) {
    // part 1: testing preemption
    auto b = new Barrier(2);
    Atomic<bool> flag1 {false};
    Atomic<bool>* flag1P = &flag1;
    
    thread([b, flag1P] {
        while (!flag1P->get()) {}
        Debug::printf("*** who is your fav character?\n");
        b->sync();
    });

    Debug::printf("*** WELCOME TO SANRIO WORLD\n");
    flag1.set(true); // ensure that the welcome message gets printed first
    b->sync();

    Debug::printf("*** cinnamoroll\n");

    // part 2: testing bb
    auto bb = new BB<int>(10);

    // testing bb when it fits the limit
    for (int i = 0; i < 10; i++) {
        bb->put(i);
    }
    for (int i = 0; i < 10; i++) {
        Debug::printf("*** %d\n", bb->get());
    }

    auto b2 = new Barrier(3);

    // testing bb when it reaches capacity
    thread([bb, b2] {
        for (int i = 0; i < 20; i++) {
            bb->put(i);
        }
        b2->sync();
    });

    thread([bb, b2] {
        int sum = 0;
        for (int i = 0; i < 10; i++) {
            sum += bb->get();
        }
        Debug::printf("*** hooray %d\n", sum);
        b2->sync();
    });

    b2->sync();

    // part 3: testing sleep
    int n = 100;

    auto b3 = new Barrier(n + 1);
    
    for (int i = 0; i < n; i++) {
        thread([b3] {
            sleep(1);
            count.fetch_add(1);
            b3->sync();
        });
    }

    b3->sync();
    Debug::printf("*** %d\n", count);
}