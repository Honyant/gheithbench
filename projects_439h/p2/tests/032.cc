#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"
#include "random.h"



/*
Memory stress test (freeing TCBs isn't enough)
The problem - when you clean up TCBs, you also need to clean up the fields it stores, particularly, the closure. This test case creats a bunch of threads with a lot of garbage in the closure. So, the destructor must be called in order to clean it up. So, to pass this, one has to - 
1. Free the TCBs at some point, preferrably right after stopping when guaranteed it won't be executed again.
2. Instead of just freeing the TCB, delete the TCB so that the destructor is called.
(ain't that deep. Instead of free(tcb);, delete tcb;)

Behavior - 
I expected a memory limit exceeded error for improper clean up. However, this test case will timeout instead. My best guest is that leaving so much garbage on the heap makes allocation a very slow process. Either way, pretty much the only way to fail this test case is to have something fundamentally wrong with your threads or barriers, or failing to free "expired" TCBs.
*/

Atomic<int> destructed_objects = 0;
int masterid = 0;

class Func {
    bool* deleted;
    int* arr;
    Barrier* barrier;
    int id; // mostly a debugging var to track threads

public:

    Func(Barrier* b, int id) : barrier(b), id(id) {
        arr = new int[1024];
        deleted = (bool*)malloc(sizeof(bool));
        *deleted = false;
    }


    void operator() () {
        // do some work. or even do nothing for all I care.
        barrier->sync();
        stop();
    }

    Func(Func const& func) {
        arr = func.arr;
        deleted = func.deleted;
        id = func.id;
        barrier = func.barrier;
    }

    ~Func() {
        if (!(*deleted)) delete [] arr;
        *deleted = true;
        destructed_objects.add_fetch(1);
    }
};

void kernelMain(void) {

    // make a set of threads, wait for them to be destroyed make the set again.

    const int N_THREADS = 100;
    const int N_ITERS = 10;


    Barrier* b = nullptr;

    for (int i = 0; i < N_ITERS; i++) {
        b = new Barrier(N_THREADS + 1);
        for (int j = 0; j < N_THREADS; j++) {
            thread(*(new Func(b, masterid++)));
        }
        b->sync();
        while (destructed_objects.get() < N_THREADS * (i+1) - 4) yield();

        // Leaking the barrier but that's ok.
    }
    Debug::printf("*** Goppert bagel ;)\n");
}