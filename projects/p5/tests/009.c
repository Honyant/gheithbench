#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define M 20
#define numPtrs 100000

#define HEAP_SIZE 1024 // (1 << 10)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

void* ptr1;
void* ptr2;
void* ptr3;

// no malloc'ed pointer should be equal to 0, unless it's malloc(0), which we account for in test 1
void checkNonNULL(void* currentPtr) {
    if (currentPtr == NULL) {
        printf("currentPtr is NULL (0)");
    }
}

// per ed #315, every pointer returned should be a multiple of 16.
void checkByteAlignment(void* currentPtr) {
    if (((uint64_t)currentPtr) % 16 != 0) {
        printf("ptr %ld isn't 16-byte aligned", (uint64_t)currentPtr);
    }
}

void checkPtr(void* currentPtr) {
    checkNonNULL(currentPtr);
    checkByteAlignment(currentPtr);
}

// stress not: no stress test!
int main() {

    // test 1: malloc(0 bytes)
    // you may either return NULL or a unique ptr that can be freed, per ed #313
    ptr1 = malloc(0);
    if (ptr1 != NULL) {
        // not sure if i can do this
        void* ptr2 = malloc(0);
        if (ptr1 != ptr2) {
            printf("failed test 1: malloc(0) doesn't give a unique ptr value, and instead varies per call");
        }
        checkPtr(ptr1);
        // we should be able to free this ptr if it's not null
        free(ptr1);
        free(ptr2);
    }

    // test 2: malloc different sizes, then free them
    ptr1 = malloc(sizeof(char));
    checkPtr(ptr1);
    free(ptr1);
    
    ptr1 = malloc(sizeof(short));
    checkPtr(ptr1);
    free(ptr1);

    ptr1 = malloc(sizeof(int));
    checkPtr(ptr1);
    free(ptr1);

    ptr1 = malloc(sizeof(long));
    checkPtr(ptr1);
    free(ptr1);

    // i'm not sure what other types we support like float, double, long long int...

    // test 3: basic int allocation, freeing, reading, writing test
    ptr1 = malloc(sizeof(int));
    *((int *) ptr1) = 100;
    checkPtr(ptr1);
    if (*((int *) ptr1) != 100) {
        printf("failed test 3: can't read/write correct value from ptr");
    }
    free(ptr1);
    
    // test 4: free block combination testing
    // tests whether you combine consecutive freed blocks

    ptr1 = malloc(128/8);
    ptr2 = malloc(128/8);
    ptr3 = malloc(128/8);
    checkPtr(ptr1);
    checkPtr(ptr2);
    checkPtr(ptr3);
    free(ptr1);
    free(ptr2);
    free(ptr3);
    // will need a block of size 512, which won't be possible given the
    // heap size of 1024 unless the blocks are combined again.
    ptr1 = malloc(512/8);
    checkPtr(ptr1);
    free(ptr1);

    printf("passed all tests!");

    return 0;
}
