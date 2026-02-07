#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define HEAP_SIZE (1 << 20)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];


int main() {
    void* last[5000] = {};

    for (uint32_t i=0; i<5000; i++) {
        last[i] = 0;
    }

    // test different byte sizes
    // checks the start and end of memory allocated
    for (size_t i = 0; i <= 1000; i++) {
        last[(int) i] = malloc(i);
        if (i >= 16) {
            *((long*) last[(int) i]) = rand() % 100000;
            *((long*) last[(int) i] + i/8-1) = -1 * (rand() % 100000);
        }
    }
    for (int i = 0; i <= 1000; i++) {
        free(last[i]);
    }

    printf("different byte sizes freed/malloced\n");

    
    // test memory reuse
    // tests repeatedly mallocing/freeing 10% of heap size repeatedly
    for (int rep = 0; rep <= 10000; rep++) {
        last[1000] = malloc(104857);
        free(last[1000]);
    }
    printf("memory reused\n");


    // test alignment of pointer returned
    for (int i = 0; i <= 10000; i++) {
        size_t blk = rand() % 10000;
        size_t* ptr = (size_t*) malloc(blk);
        if (((size_t) ptr) % 16 != 0) {
            printf("pointer allocation not aligned\n");
            break;
        }
        free(ptr);
    }
    printf("pointer allocation aligned\n");


    // test merges
    size_t* a1 = (size_t*) malloc(rand()%1000);
    size_t* b1 = (size_t*) malloc(rand()%1000);
    size_t* c1 = (size_t*) malloc(rand()%1000);
    // test freeing block: (not free) block (not free)
    free(b1);

    size_t* a2 = (size_t*) malloc(rand()%1000);
    size_t* b2 = (size_t*) malloc(rand()%1000);
    size_t* c2 = (size_t*) malloc(rand()%1000);
    // test freeing block: (not free) block (free)
    free(c2);
    free(b2);

    size_t* a3 = (size_t*) malloc(rand()%1000);
    size_t* b3 = (size_t*) malloc(rand()%1000);
    size_t* c3 = (size_t*) malloc(rand()%1000);
    // test freeing block: (free) block (not free)
    free(a3);
    free(b3);

    size_t* a4 = (size_t*) malloc(rand()%1000);
    size_t* b4 = (size_t*) malloc(rand()%1000);
    size_t* c4 = (size_t*) malloc(rand()%1000);
    // test freeing block: (free) block (free)
    free(a4);
    free(c4);
    free(b4);

    // free leftover memory
    free(c3); free(a2); free(a1); free(c1);  
    printf("different merge types tested\n");

    return 0;
}