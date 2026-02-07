#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define HEAP_SIZE (1 << 10)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

int main() {
    long* allocs[100];
    for (uint32_t i=0; i<100; i++) {
        allocs[i] = 0;
    }

    //Test 1: checking malloc and free counter incremented for unsuccessful calls
    long m1 = mCount;
    long f1 = fCount;
    allocs[0] = malloc(0);
    free(0);
    long m2 = mCount - m1;
    long f2 = fCount - f1;
    if (m2 != 1) {
        panic("Incorrectly incremented malloc counter for unsuccessfull calls by %ld\n", m2);
    }
    if (f2 != 1) {
        panic("Incorrectly incremented free counter for unsuccessfull calls by %ld\n", f2);
    }

    //Test 2: check alignment
    allocs[0] = malloc(64);
    allocs[1] = malloc(32);
    allocs[2] = malloc(16);
    allocs[3] = malloc(8);
    allocs[4] = malloc(4);
    allocs[5] = malloc(2);
    allocs[6] = malloc(1);
    for (int i = 0; i <= 6; i++) {
        if((long) allocs[i] % 16 != 0){
            int blockSize = 64 / (1 << i);
           panic("Allocated block of size %d not 16 byte aligned\n", blockSize);
        }
    }

    //Test 3: check no heap overlap
    for (int i = 0; i <= 5; i++) {
        char* ptr1 = (char*)allocs[i];
        char* ptr2 = (char*)allocs[i + 1];
        //checking pointer address difference
        if (ptr2 >= ptr1) {
            if (ptr2 - ptr1 < 64 / (1 << (i + 1))) {
                int blockSize = 64 / (1 << i);
                panic("Heap overlap occured between %d-byte and %d-byte size block\n", blockSize, blockSize << 1);
            }
        } else {
            if (ptr1 - ptr2 < 64 / (1 << (i + 1))) {
                int blockSize = 64 / (1 << i);
                panic("Heap overlap occured between %d-byte and %d-byte size block\n", blockSize, blockSize << 1);
            }
        }
    }
    
    //Test 4: check consecutive frees
    for (int i = 0; i <= 6; i++) {
        free(allocs[i]);
    }
    m2 = mCount - m1;
    f2 = fCount - f1;
    if (m2 != 8) {
        panic("Incorrectly incremented cumulative malloc counter by %ld\n", m2);
    }
    if (f2 != 8) {
        panic("Incorrectly incremented cumulative free counter by %ld\n", f2);
    }


    //Test 5: check freeing at start/end of heap and working inwards
    for (int i = 0; i < 100; i++) {
        allocs[i] = malloc(8);
    }
    for (int i = 0; i < 50; i++) {
        free(allocs[i]);
        free(allocs[99-i]);
    }
    m2 = mCount - m1;
    f2 = fCount - f1;
    if (m2 != 108) {
        panic("Incorrectly incremented cumulative malloc counter by %ld\n", m2);
    }
    if (f2 != 108) {
        panic("Incorrectly incremented cumulative free counter by %ld\n", f2);
    }
    
    //Test 6: check for memory fragmentation, malloc should return a valid pointer
    //A segfault likely indicates fragmentation
    char* pointer = malloc(64);
    for (int i = 0; i < 64; i++) {
        pointer[i] = 1;
    }

    printf("Congratulations, your mallocing and freeing makes you a proficient heap manager\n");

    return 0;
}

