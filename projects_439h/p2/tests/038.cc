#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "smp.h"
#include "barrier.h"
#include "promise.h"
// #include <string>

/* test 1 - sum an array in parallel by using different threads to sum different chunks 
 * tests yield(), stop(), barrier)*/
const int ARRLEN=50000;
int globalArray[ARRLEN]; //global array of 1 to ARRLEN
const int NUMTHREADS=10; //we'll be using 10 threads :D
static int threadSums[NUMTHREADS] = {0}; //per thread sums
const int SUBLEN=ARRLEN/NUMTHREADS;

//helper function to sum a chunk of the array
void sumArrayPart(int threadId, int start, int end, int step, Barrier* b1) {
    int localSum = 0;
    int chunkSize = (end - start) / step;

    for (int iter = 0; iter < step; iter++) {
        int iterStart = start + iter * chunkSize;
        int iterEnd = (iter == step - 1) ? end : iterStart + chunkSize;
        //sum the chunk
        for (int i = iterStart; i < iterEnd; i++) {
            localSum += globalArray[i];
        }
        yield();
    }
    threadSums[threadId] = localSum;
    b1->sync(); //end of a thread's work -> wait at barrier for the other threads
}

/* Called by one core */
void kernelMain(void) {
    Debug::printf("*** hi!\n");

    //make array of 1 to 50000
    for (int i = 0; i < ARRLEN; i++) {
        globalArray[i] = i + 1;
    }

    //call sumarraypart, each thread is responsible for adding a diff part
    Barrier* b1 = new Barrier(NUMTHREADS+1);
    for (int i = 0; i < NUMTHREADS; i++) {
        thread([b1, i]{
            int start = i * SUBLEN;
            int end = (i == NUMTHREADS-1) ? ARRLEN : start + SUBLEN;
            sumArrayPart(i, start, end, NUMTHREADS, b1);
        });
    }
    b1->sync();

    //get total sum
    long globalSum = 0;
    for (int i = 0; i < NUMTHREADS; i++) {
        globalSum += threadSums[i];
    }
    Debug::printf("*** array sum = %d\n", globalSum);

    /*test 2 - test promises!*/
    // const int NUMTHREADS2 = 6;
    Barrier* b2 = new Barrier(4);
    Promise<char> promise;
    Promise<int> promise2;
    thread([&promise,&promise2,b2]{
        Debug::printf("*** I hope you keep your promise...\n");
        char value = promise.get();
        int value2 = promise2.get();
        Debug::printf("*** yay you kept your promise!!\n");
        Debug::printf("*** you promised me: %c%d\n", value, value2);
        b2->sync();
    });
    thread([&promise2,b2]{
        promise2.set(3);
        b2->sync();
    });
    thread([&promise,b2]{
        promise.set('<');
        b2->sync();
    });
    b2->sync();
    
    Debug::printf("*** byebye!\n");
}

