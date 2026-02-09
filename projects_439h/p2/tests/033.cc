#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"
#include "promise.h"

//assuming there's 4 cores
//if there aren't 4 cores the output will be the same
//if there are less, intended behavior will still be tested
//if there are more, output will be same and intended behavior will not be tested
//if I used kConfig.totalProcs the .ok file would have to auto generate or something 
//hopefully simple barrier and promise test case
void kernelMain(void)
{
    //testing barriers
    //make 4 threads so cores have to block not spin
    //so there's 5 with kernel main

    //n is number of intended core
    //read comments above
    const uint32_t n = 4;
    Barrier barrier = Barrier(n+1);
    Barrier barrier2 = Barrier(n+1);
    Barrier barrier3 = Barrier(n+1);

    auto non_mains = [&barrier, &barrier2, &barrier3] {
        Debug::printf("*** hi\n");
        barrier.sync();
        //kernel main prints here
        barrier2.sync();
        Debug::printf("*** bye\n");
        barrier3.sync();
        //Debug::shutdown();
    };

    for (uint32_t i = 0; i < n; i++) {
        thread(non_mains);
    }

    barrier.sync();

    Debug::printf("*** hi from kernel main\n");

    barrier2.sync();
    //"bye"s print here
    barrier3.sync();


    //testing promises
    //making 5 so cores have to block not spin assuming 4 cores
    Promise<int> a_promise = Promise<int>();
    Barrier *promise_barrier = new Barrier(n+1);


    for (uint32_t i = 0; i < n; i++) {
        thread([&a_promise, promise_barrier] {
            auto val = a_promise.get();
            Debug::printf("*** %d\n", val);
            promise_barrier->sync();
        });
    }
    //should have 4 threads waiting for the promise
    thread([&a_promise] {
        a_promise.set(31);
        //don't need to sync here since others can't continue until promise is set
    });
    //now it's been set
    //don't want kernel main to finish and shutdown so another barrier
    promise_barrier->sync();
}