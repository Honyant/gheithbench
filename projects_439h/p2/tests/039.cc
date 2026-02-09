#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"
#include "promise.h"

/*
 * Test that utilizes promises for arithmetic and barriers for synchronizing printing. 
 * Each promise represents a sequence number in an arithmetic sequence, where the 100th number
 * in the sequence will be printed out (synchronized by the barriers), and then the total sum as well
 */

/* Called by one core */
void kernelMain() {
    Debug::printf("*** hello\n");

    int32_t barrierSize = 201; // Number of threads calling "sync()" to compute sum
    int32_t numPromises = 100; // total number of promises = numPromises * 2 (and also barrier size)
    Promise<int32_t>* sequenceNums[numPromises * 2 + 1];
    Barrier* sequenceBarrier = new Barrier(barrierSize);

    // compute each value in the sequence with promises
    for (int32_t i = 0; i < numPromises * 2; i++) {
        int32_t j = i + 1;
        sequenceNums[i] = new Promise<int32_t>();
        thread([&sequenceNums, i, j, sequenceBarrier] {
            sequenceNums[i]->set(j);
            sequenceBarrier->sync();
        });       
    }
    sequenceBarrier->sync();


    // Print out the sum of the whole arithmetic sequence (which is the numbers from 1...200 inclusive)
    Barrier* sumBarrier = new Barrier(barrierSize);
    Atomic<int32_t> sum{0};
    for (int32_t i = 0; i < numPromises * 2; i++) {
        int32_t currNum = sequenceNums[i]->get();
        thread([currNum, sumBarrier, &sum] {
            sum.add_fetch(currNum);
            sumBarrier->sync();
        });
    }
    sumBarrier->sync();
    // print out the 100th element in the sequence (index 99 since 0 based)
    Debug::printf("*** sequence number=%d; sequence value=%d\n", 100, sequenceNums[99]->get());
    Debug::printf("*** total sum of sequence is = %d\n", sum.get());
}
