#include "threads.h"
#include "debug.h"
#include "promise.h"
#include "barrier.h"


void kernelMain(void) {
    Debug::printf("*** Welcome to this test\n");
    Promise<double>* val = new Promise<double>();

    //tests that calling "thread" does not cause core to switch threads (the ordering of print statements is enforced)
    //the request for the unavailable promise forces the context switch to this thread, which then sets that promise
    thread([val] {
        Debug::printf("*** Promise not ready, context switch forced\n");

        //this tests that threads can create their own threads
        //this thread needs to be scheduled and executed so that the promise is set
        thread([val] {
            Debug::printf("*** This thread was created inside another thread\n");
            val->set(3.14);
        });
        
    });

    Debug::printf("*** %.2f\n", val->get());

    Promise<double>* x1 = new Promise<double>();
    Promise<double>* x2 = new Promise<double>();
    Promise<double>* x3 = new Promise<double>();
    Promise<double>* x4 = new Promise<double>();

    //tests that barrier's sync function performs correct tracking within threads that yield between each other
    Barrier* barrier = new Barrier(4);

    //Let's create four threads
    //This tests yield() function and that we properly schedule all declared threads
    //All the threads should be scheduled and run for correct sum of promises
    //yield() and sync() are called in various orders with respect to where promise is set in the thread
    //this should NOT change the behavior since the core should pick up another one of the available threads
    thread([x1, barrier] {
        barrier->sync();
        yield();
        x1->set(1);
        
    });
    thread([x2, barrier] {
        x2->set(2);
        barrier->sync();
        yield();
        
    });
    thread([x3, barrier] {
        yield();
        x3->set(3);
        barrier->sync();
        
    });
    thread([x4, barrier] {
        barrier->sync();
        x4->set(4);
        yield();
    });

    //tests that we can yield from kernelMain
    //also forces a context switch to one of the above threads
    yield();


    //ensures above threads were all properly scheduled and executed:
    //tests proper scheduling of threads while the promises are resolved 
    int sum = x1->get() + x2->get() + x3->get() + x4->get();
    Debug::printf("*** %ld\n", sum);

    

}