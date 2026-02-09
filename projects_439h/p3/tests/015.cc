#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"
#include "bb.h"

#define T15_BARRIER_SIZE 350

/**
 * This test case stresses your implementation
 * of BB to make sure you designed it with 
 * speed in mind. It will cause tons of threads 
 * to wait, so you need to make sure it 
 * is able to handle that efficiently.
 */
void kernelMain(void) {
    Debug::printf("*** Starting\n");
    auto cnter = new Barrier(T15_BARRIER_SIZE+1);
    auto bb = new BB<int>(T15_BARRIER_SIZE);
    
    for(int i = 0; i < T15_BARRIER_SIZE; i++) {
      thread([bb, cnter, i] {
        int v = i;
        cnter->sync();
        bb->put(v);
      });
    }

    // Note: everything may not finish entering bb 
    // by the time cnter gets to this limit, but
    // this is OK because it still will test 
    // what we are trying to test due to the magnitude
    cnter->sync();
    for(int i = 0; i < T15_BARRIER_SIZE; i++) {
      bb->get();
    }
  
    Debug::printf("*** Yay! Good job\n");
}

