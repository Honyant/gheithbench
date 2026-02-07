#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define HEAP_SIZE 1024

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

int main() {
    // this test will test your holes. it will test your merging of the holes
    // it will also test that you are using best fit not first fit (whichever one is better is up for debate so this test case should really not be chosen) 
    // this isn't a very strict test case: your program should pass with 32 bytes of metadata

    long m1 = mCount;
    long f1 = fCount;

    size_t blockSize = 96;
    void *ptrs[8];

    // TESTING MERGING HOLES:
    // fill heap with 8 blocks of size 96 (if 32 metadata then each node is 128 bytes)
    for (int i = 0; i < 8; i++) {
        ptrs[i] = malloc(blockSize);
        if (!ptrs[i]) {
            panic("Failed hole test, first allocation failed.\n");
        }
    }

    // free some blocks
    free(ptrs[1]);
    free(ptrs[4]);
    free(ptrs[6]);
    free(ptrs[7]);
    free(ptrs[0]);
    free(ptrs[3]);

    void *bigPtrs[3];
    // if merged the free blocks, should be enough space to allocate
    // also testing alignment here
    // big blocks are size 113 + padding
    for (int i = 0; i < 3; i++) {
        bigPtrs[i] = malloc(blockSize + 17);
        if (!ptrs[i]) {
            panic("Failed hole test, second allocation failed.\n");
        }
    }

    // free everything
    free(ptrs[2]);
    free(ptrs[5]);
    for (int i = 0; i < 3; i++) {
        free(bigPtrs[i]);
    }

    // TESTING BEST FIT:
    // same thing: fill heap with 8 blocks of size 96 (if 32 metadata then each node is 128 bytes)
    for (int i = 0; i < 8; i++) {
        ptrs[i] = malloc(blockSize);
        if (ptrs[i] == NULL) {
            panic("Failed best fit test, first allocation failed.\n");
        }
    }

    free(ptrs[0]);
    free(ptrs[1]);
    free(ptrs[3]);

    char *newSmallPtr = malloc(blockSize);
    if (newSmallPtr == NULL) {
        panic("Failed best fit test, second allocation failed.\n");
    }

    char *newBigPtr = malloc(blockSize + 17);
    if (newSmallPtr == NULL) {
        panic("Failed best fit test, third allocation failed. You probably aren't using best fit\n");
    }

    free(ptrs[2]);
    free(ptrs[4]);
    free(ptrs[5]);
    free(ptrs[6]);
    free(ptrs[7]);
    free(newSmallPtr);
    free(newBigPtr);
    
    long m2 = mCount - m1;
    long f2 = fCount - f1;
  
    if (m2 != f2) {
        printf("m2 %ld\n",m2);
        printf("f2 %ld\n",f2);
    } else {
        printf("count match\n");
    }

    if (m2 != 21) {
        printf("*** wrong count %ld\n",m2);
    } else {
        printf("count ok\n");
    }

    return 0;
}
