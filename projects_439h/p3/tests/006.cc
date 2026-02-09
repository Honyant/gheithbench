#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "bb.h"
#include "blocking_lock.h"

BB<int>* buffer = new BB<int>(3);
uint32_t numCores = kConfig.totalProcs;
const uint32_t matrix_size = 10;
uint32_t mat[matrix_size][matrix_size];

void kernelMain(void) {
    // This test case relies on preemption in that it creates infinite loops that 
    // can only be avoided with preemption. 
    for (uint32_t i = 0; i < numCores; i++){
        thread([i] {
            while (true) ;
        });
    }
    yield(); // forcing kernelMain to yield to an infinite loop thread
    Debug::printf("*** Made it out of infinite loop threads!\n");

    // sanity check on bounded buffer
    int a = 1;
    buffer->put(a); 
    if (buffer->get() != a) {
        Debug::printf("*** there is an issue with bounded buffer\n");
    }

    // testing that a buffer makes room for new additions when another item is removed 
    buffer->put(a);
    thread([] {
        for (int i = 2; i < 4; i++){
            buffer->put(i);
        }
    });
    buffer->get(); // should retrieve a 1, making room for 2 and 3 to be pulled
    while (buffer->get() != 3) yield();
    Debug::printf("*** Made it out of the bounded buffer\n");


    
    // now we will perform a bunch of operations on a shared matrix
    // this is inspired by a dynamic programming style matrix that
    // can leverage multithreading

    // we start by filling the matrix with ones 
    for (uint32_t i = 0; i < matrix_size; i++){
        for (uint32_t j = 0; j < matrix_size; j++){
            mat[i][j] = 1;
        }
    }

    // now fill the first row and first column with twos
    for (uint32_t i = 0; i < matrix_size; i++){
        mat[0][i] = 2; 
        mat[i][0] = 2; 
    }

    // now make threads to fill in the rest of the matrix in a DP style
    for (uint32_t i = 1; i < matrix_size; i++){
        for (uint32_t j = 1; j < matrix_size; j++){
            thread ([i, j] {
                // each thread waits for its left, up, and left-up diagonal neighbors 
                // to be filled. this depends on preemption
                while (mat[i-1][j-1] == 1) ; 
                while (mat[i][j-1] == 1) ; 
                while (mat[i-1][j] == 1) ;

                mat[i][j] = 2; // once those neighbors are filled, then so is this cell. 
            });
        }
    }

    while (mat[matrix_size-1][matrix_size-1] != 2) ;

    Debug::printf("*** Made it out of the DP array!\n");
}

