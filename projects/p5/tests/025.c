#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define HEAP_SIZE 8192

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE / sizeof(long)];

bool aligned(void* ptr) {
    return (long)ptr % 16 == 0;
}

int main() {
    void* ptrs[1000];
    ptrs[0] = malloc(0); // should be valid, can't assume anything about the result though

    for (int i = 0; i < 50; i++) {
        // if we assume 32 bytes of metadata per allocation, should use about 64 bytes of memory
        ptrs[i] = malloc(20); 
        if (!aligned(ptrs[i])) {
            panic("MALLOC ON LINE 20 NOT ALIGNED");
        }
    }
    for (int i = 0; i < 50; i++) {
        free(ptrs[i]);
    }

    // duplicate code as above to ensure we really freed (and merged) everything
    for (int i = 0; i < 50; i++) {
        // if we assume 32 bytes of metadata per allocation, should use about 64 bytes of memory
        ptrs[i] = malloc(20); 
    }
    for (int i = 0; i < 50; i++) {
        free(ptrs[i]);
    }

    // testing malloc on different sizes
    for (int i = 0; i < 100; i++) {
        ptrs[i] = malloc(i + 1);
        
        // store something in the last accessible memory address to ensure we are given enough memory
        ((char*)ptrs[i])[i] = 10;
    }
    for (int i = 0; i < 100; i++) {
        if (((char*)ptrs[i])[i] != 10) {
            panic("VALUE NOT SAVED (check line 46)");
        }
        free(ptrs[i]);
    }

    printf("Yay! You passed!!\n");
}