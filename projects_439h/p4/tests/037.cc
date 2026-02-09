#include "debug.h"
#include "atomic.h"
#include "promise.h"
#include "barrier.h"
#include "bb.h"

int ans = 10;
Atomic<bool> done1{false};
Atomic<bool> done2{false};
Atomic<bool> done12{false};
Atomic<bool> done22{false};

//test that you always remember to re-enable interrupts properly w synchronization primitives
//sprinkle in an ASSERT every line just because
//this is what I've been doing to debug so maybe it'll help someone

void kernelMain(void) {

    //section 1: assert that interrupts are enabled


    //part 1 : relating to promises
    ASSERT(!Interrupts::isDisabled());
    Debug::printf("*** part 1!\n");
    ASSERT(!Interrupts::isDisabled());
    auto p = new Promise<int>();
    ASSERT(!Interrupts::isDisabled());
    for (int i = 0; i < ans; i++) {
        thread([p, i] {
            ASSERT(!Interrupts::isDisabled());
            int a = p->get();
            ASSERT(!Interrupts::isDisabled());
            if (i == ans - 1) {
                ASSERT(!Interrupts::isDisabled());
                Debug::printf("*** answer = %d\n", a);
                ASSERT(!Interrupts::isDisabled());
                done1.set(true);
                ASSERT(!Interrupts::isDisabled());
                done2.set(true);
                ASSERT(!Interrupts::isDisabled());
            }
            ASSERT(!Interrupts::isDisabled());
        });
    }
    ASSERT(!Interrupts::isDisabled());
    p->set(ans); //has to run before gets can run
    ASSERT(!Interrupts::isDisabled());
    while (!done1.get()) {
        ASSERT(!Interrupts::isDisabled());
    }

    //part 2: relating to barriers
    Debug::printf("*** part 2!\n");
    auto b = new Barrier(ans + 1);
    ASSERT(!Interrupts::isDisabled());
    for (int i = 0; i < ans; i++) {
        thread([b, i] {
            ASSERT(!Interrupts::isDisabled());
            b->sync();
            ASSERT(!Interrupts::isDisabled());
        });
    }
    ASSERT(!Interrupts::isDisabled());
    b->sync();
    ASSERT(!Interrupts::isDisabled());
    while(!done2.get()) {
        ASSERT(!Interrupts::isDisabled());
    }
    Debug::printf("part 2 passed!\n");
    
    //part 3: relating to bounded buffers
    Debug::printf("*** part 3!\n");
    auto bb = new BB<int>(ans);
    ASSERT(!Interrupts::isDisabled());
    thread([bb] {
        ASSERT(!Interrupts::isDisabled());
        for (int i = 0; i < ans; i++) {
            ASSERT(!Interrupts::isDisabled());
            bb->put(i);
            ASSERT(!Interrupts::isDisabled());
        }
        ASSERT(!Interrupts::isDisabled());
    });

    ASSERT(!Interrupts::isDisabled());
    for (int i = 0; i < ans; i++) {
        ASSERT(!Interrupts::isDisabled());
        int a = bb->get();
        ASSERT(!Interrupts::isDisabled());
        ASSERT(i == a);
        ASSERT(!Interrupts::isDisabled());
    }
    ASSERT(!Interrupts::isDisabled());
    Debug::printf("*** bb successful!\n");

    //section 2: the same things but with interrupts DISABLED in every thread
    Interrupts::disable();

    //part 4: promises again
    ASSERT(Interrupts::isDisabled());
    Debug::printf("*** part 4!\n");
    ASSERT(Interrupts::isDisabled());
    auto p2 = new Promise<int>();
    ASSERT(Interrupts::isDisabled());
    for (int i = 0; i < ans; i++) {
        ASSERT(Interrupts::isDisabled());
        thread([p2, i] {
            Interrupts::disable();
            ASSERT(Interrupts::isDisabled());
            int a = p2->get();
            ASSERT(Interrupts::isDisabled());
            if (i == ans - 1) {
                ASSERT(Interrupts::isDisabled());
                Debug::printf("*** answer = %d\n", a);
                ASSERT(Interrupts::isDisabled());
                done12.set(true);
                ASSERT(Interrupts::isDisabled());
                done22.set(true);
                ASSERT(Interrupts::isDisabled());
            }
            ASSERT(Interrupts::isDisabled());
        });
        ASSERT(Interrupts::isDisabled());
    }
    ASSERT(Interrupts::isDisabled());
    p2->set(ans); //has to run before gets can run
    ASSERT(Interrupts::isDisabled());
    while (!done12.get()) {
        ASSERT(Interrupts::isDisabled());
    }

    //part 5: barriers again
    ASSERT(Interrupts::isDisabled());
    Debug::printf("*** part 5!\n");
    ASSERT(Interrupts::isDisabled());
    auto b2 = new Barrier(ans + 1);
    ASSERT(Interrupts::isDisabled());
    for (int i = 0; i < ans; i++) {
        ASSERT(Interrupts::isDisabled());
        thread([b2, i] {
            Interrupts::disable();
            ASSERT(Interrupts::isDisabled());
            b2->sync();
            ASSERT(Interrupts::isDisabled());
        });
        ASSERT(Interrupts::isDisabled());
    }
    ASSERT(Interrupts::isDisabled());
    b2->sync();
    ASSERT(Interrupts::isDisabled());
    while(!done2.get()) {
        ASSERT(Interrupts::isDisabled());
    }
    Debug::printf("part 5 passed!\n");
    ASSERT(Interrupts::isDisabled());

    //part 6: bb again
    Debug::printf("*** part 6!\n");
    ASSERT(Interrupts::isDisabled());
    auto bb2 = new BB<int>(ans);
    ASSERT(Interrupts::isDisabled());
    thread([bb2] {
        Interrupts::disable();
        ASSERT(Interrupts::isDisabled());
        for (int i = 0; i < ans; i++) {
            ASSERT(Interrupts::isDisabled());
            bb2->put(i);
            ASSERT(Interrupts::isDisabled());
        }
        ASSERT(Interrupts::isDisabled());
    });

    ASSERT(Interrupts::isDisabled());
    for (int i = 0; i < ans; i++) {
        ASSERT(Interrupts::isDisabled());
        int a = bb2->get();
        ASSERT(Interrupts::isDisabled());
        ASSERT(i == a);
        ASSERT(Interrupts::isDisabled());
    }
    ASSERT(Interrupts::isDisabled());
    Debug::printf("*** bb2 successful!\n");

}