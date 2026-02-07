#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define M 200
#define N 1

#define HEAP_SIZE (1 << 20)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

void* last[M] = {};

int main() {

    for (uint32_t i=0; i<M; i++) {
        last[i] = 0;
    }

    // assuming header < 1000 bytes
    // malloc 200 blocks of 4000 bytes each
    for (uint32_t i=0; i<M; i++) {
        last[i] = malloc(4000);
    }

    // test merging blocks
    for (uint32_t i = 0; i < (M / 2); i+=2) {
        // free adjacent blocks
        free(last[i]);
        free(last[i+1]);
        // malloc a slightly bigger block
        last[i] = malloc(4200);
    }


    printf("done\n");

    return 0;
}