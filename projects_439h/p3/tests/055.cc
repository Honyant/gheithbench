#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "bb.h"
#include "barrier.h"

//This test case tests a bunch of stuff in your bounded buffer implementation.

/* Called by one core */
void kernelMain(void) {

    //Part 1
    int p1_n = 100;
    BB<int> *bb1 = new BB<int>(1);
    Atomic<int> count = 0;
    Barrier *b = new Barrier(p1_n * 2 + 1);

    //Call a bunch of gets and a bunch of puts.
    for (int i = 0; i < p1_n; i++) {
        thread([b, &count, bb1] {
            count.add_fetch(bb1->get());
            b->sync();
        });
        thread([b, i, bb1] {
            int j = i;
            bb1->put(j);
            b->sync();
        });
    }

    b->sync();
    int final_count = count.get();
    Debug::printf("*** Final count was %d\n", final_count); //should've added all numbers from 0 to 99. 
    Debug::printf("*** Part 1 finished\n");

    //Part 2
    BB<int> *bb2 = new BB<int>(1);
    Barrier *b2 = new Barrier(2);

    thread([bb2, b2] {
        for (int i = 0; i < 15; i++) {
            bb2->put(i);
        }
        b2->sync();
    });
    for (int i = 0; i < 15; i++) {
        Debug::printf("*** Got %d\n", bb2->get());
    }
    b2->sync();
    Debug::printf("*** Part 2 finished\n");
    
    thread([bb2] {
        bb2->get();
        Debug::printf("*** This shouldn't print bc this thread should be blocked forever\n");
    });

    //Part 3
    BB<int> *bb3 = new BB<int>(1);
    Barrier *b3 = new Barrier(2);

    thread([bb3, b3] {
        for (int i = 0; i < 15; i++) {
            Debug::printf("*** Got %d\n", bb3->get());
        }
        b3->sync();
    });
    for (int i = 0; i < 15; i++) {
        bb3->put(i);
    }
    b3->sync();
    Debug::printf("*** Part 3 finished\n");
    
    thread([bb3] {
        int i = 1;
        bb3->put(i);
        bb3->put(i);
        Debug::printf("*** This also shouldn't print bc this thread should also be blocked forever\n");
    });

}

