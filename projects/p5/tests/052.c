#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "heap/heap.h"
#include "heap/panic.h"
#include <stdint.h>

#define HEAP_SIZE 32768
#define NUM_RANDOM 500
#define N (NUM_RANDOM + 1)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE / sizeof(long)];

void* ptrs[NUM_RANDOM];

bool isAligned(void* ptr) {
    return (uint64_t)ptr % 16 == 0;
}


void testLarge() { // tries to allocate a large chunk of memory
    ptrs[0] = malloc(32000);
    if(ptrs[0] == NULL) {
        panic("large malloc is not fitting\n");
    }
    if(!isAligned(ptrs[0])) {
        panic("large malloc is not aligned\n");
    }
    free(ptrs[0]);
}


void testRandom() { // testing some random sizes for alignment and space
    for(int i = 0; i < NUM_RANDOM; i ++) {
        size_t sz = rand() % 28;
        ptrs[i] = malloc(sz);
        if(!isAligned(ptrs[i])) {
            panic("random malloc is not aligned\n");
        }
        if(ptrs[i] == NULL) {
            panic("malloc'ing random gave back NULL (may have ran out of space)\n");
        }
    }
    
    for(int i = 0; i < NUM_RANDOM; i ++) {
        free(ptrs[i]);
    }
}


int main() {

    long m1 = mCount;
    long f1 = fCount;

    testLarge();
    testRandom();

    long m2 = mCount - m1;
    long f2 = fCount - f1;

    if (m2 != f2) {
        printf("m2 %ld\n",m2);
        printf("f2 %ld\n",f2);
    } else {
        printf("count match\n");
    }

    if (m2 != N) {
        printf("*** wrong count %ld\n",m2);
    } else {
        printf("count ok\n");
    }

    return 0;
}