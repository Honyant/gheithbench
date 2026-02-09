#include "bb.h"
#include "threads.h"
#include "debug.h"
#include "barrier.h"
#include "promise.h"

Barrier b{11};
Barrier b2{5};
int counter = 0;
bool test_done = false;

void kernelMain(void) {
    // testing for preemption
    for(int i = 0; i < 4; i++) {
        thread([]{
            while (!test_done);
        });
    }

    // bb lock test
    auto bb_lock = new BB<int>(1);
    for(int i = 0; i < 10; i++) {
        thread([bb_lock]{
            int t = 0;
            bb_lock->put(t); 
            Debug::printf("*** Counter: %d\n", ++counter);
            bb_lock->get();
            b.sync(); 
        });
    }
    b.sync(); 

    // passing a bb through a promise lol
    auto promise = new Promise<BB<int>*>(); 
    auto bb = new BB<int>(1);
    auto handoff_bb = new BB<int>(1); 

    for(int i = 0; i < 4; i++) {
        thread([promise, handoff_bb]{
            auto bb = promise->get(); 
            Debug::printf("*** %d\n", bb->get());
            int t = 0;
            handoff_bb->put(t);
            b2.sync();
        });
    }

    int q = 1111111;
    promise->set(bb); 
    for(int i = 0; i < 4; i++) {
        bb->put(q); q *= 2;
        handoff_bb->get();
    }
    b2.sync(); 
    
    Debug::printf("*** Done!\n");
    test_done = true;
}


