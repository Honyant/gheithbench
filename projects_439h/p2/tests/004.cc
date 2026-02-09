#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "promise.h"
#include "barrier.h"

/*
This test is designed to ensure you are handling promise race conditions (in the starter code) correctly,
and is concise so that it's easy to debug. It creates N conflicts between set and get to ensure your locking/semaphore logic is 
airtight and must be handled correctly and efficiently. To stress test your implementation more, you can increase N and change 
the .ok file accordingly. If you print anything out of order, your barrier probabily contains a race condition. I also
use multiple barriers to ensure that you don't have barrier race conditions between threads.

Run this hundreds of times to verify your barriers and promises.
*/

void kernelMain(void) {

    int N = 200; //number of gets called
    auto barrier = new Barrier(N); //ensure we get correct print output
    auto bigAnswer = new Promise<int>();

    for(int i = 1; i < N; i++){ //create n-1 threads waiting to get

        auto answer = new Promise<int>();
        auto newBarrier = new Barrier(2);

        thread([answer, newBarrier, barrier] {

            Debug::printf("*** received promise! %d\n", answer->get());    //this should wait for the first set
            newBarrier->sync();
            barrier->sync();

        });     

        thread([i, answer, newBarrier] {

            Debug::printf("*** this should go first \n"); //this should set first, guaranteeing that this line is printed
            answer->set(i);   

        });  

        newBarrier->sync();
    }

    barrier->sync();
    bigAnswer->set(200);
    
    Debug::printf("*** check your barrier doesn't time out: = %d\n",bigAnswer->get()); //lastly, we check that your barrier is implemented correctly

}