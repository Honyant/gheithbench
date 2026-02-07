#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define M 20
#define N 100000

#define HEAP_SIZE (2408) //2400 for max x50 slots of 16 w/ size-32 metadata; 10 extra (TODO: see if allocatable?)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

void checkPtr(void* ptr) {
    void* startPtr = (void*)the_heap;
    void* endPtr = startPtr + HEAP_SIZE;

    if (ptr==0) { return; } //nullptr
    
    if ((ptr-startPtr) < 0) { printf("uh oh ptr at %ld OOB? (too low)", (uint64_t)ptr); }
    if ((endPtr-ptr) < 0) { printf("uh oh ptr at %ld OOB? (too high)", (uint64_t)ptr); }

    if (((uint64_t)ptr)%16 != 0) { printf("Uh oh! %ld wasn't 16-aligned", (uint64_t)ptr); }
}

int main() {
    void* ptrArr[50] = {};

    //TEST: make 49 small allocs
    for (int i=0; i<50; i++) {
        ptrArr[i] = malloc(16);
        checkPtr(ptrArr[i]);

        (((long*)ptrArr[i])[0]) = 0;
        (((long*)ptrArr[i])[1]) = (long)40000; //write a long to the latter half?

        // if (  ) { //if metadata size = 16 not 32, hopefully //any <16 byters get a free pass

        // }

        
    }
    //TEST: alloc the extra 16slot plus 10.
    void* extraPtr = malloc(26);
    checkPtr(extraPtr);
    
    //TEST: go!!! be free!!
    for (int i=0; i < sizeof(ptrArr)/sizeof(ptrArr[0]); i++) {
        if (i%2) { free(ptrArr[i]); }
    }
    for (int i=0; i < sizeof(ptrArr)/sizeof(ptrArr[0]); i++) { //check that recombining both front and back works
        if (!(i%2)) { free(ptrArr[i]); }
    }

    //TEST: alloc big block
    void* bigPtr = malloc(2410-32);
    checkPtr(bigPtr);

    void* zeroPtr = malloc(0); //please don't crash
    if (zeroPtr) {
        //so the var is used
    }

    if (mCount!=(fCount+3)) { printf("uh oh mcount (%ld) and fcount (%ld) aren't equal", mCount, fCount); } //+3 for the extra malloc, big malloc, and the 0 malloc

    printf("ok!!");
    
	return 0;
}
