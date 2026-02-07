#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"
#include <stdbool.h>

#define HEAP_SIZE (1 << 22)

long heap_size = HEAP_SIZE;
long max_size = HEAP_SIZE - 100;
long the_heap[HEAP_SIZE/sizeof(long)];

void checkPointer(void* p) {
    if (p == 0) {
        panic("Pointer is null");
    }
    if ((long)p % 16 != 0) {
        panic("Pointer is not 16 byte aligned");
    }
}

int main() {
    // test malloc 0
    void* zeros [1000];
    for (int i = 0; i < 1000; i++) {
        zeros[i] = malloc(0);
    }
    for (int i = 0; i < 1000; i++) {
        free(zeros[i]);
    }

    long m1 = mCount;
    long f1 = fCount;

    // test merging (in order to fit the large block, these have to be properly merged)
    void* a = malloc((max_size - 100) / 3);
    void* b = malloc((max_size - 100) / 3);
    void* c = malloc((max_size - 100) / 3);
    checkPointer(a);
    checkPointer(b);
    checkPointer(c);
    free(a);
    free(b);
    free(c);

    // test malloc/free of a large block
    void* largeBlock = malloc(max_size);
    checkPointer(largeBlock);
    free(largeBlock);

    // test many mallocs and frees
    void* blocks [1000];
    for (int i = 0; i < 1000; i++) {
        blocks[i] = malloc(rand() % 1000 + 1);
        checkPointer(blocks[i]);
    }
    for (int i = 0; i < 1000; i++) {
        free(blocks[i]);
    }

    long m2 = mCount - m1;
    long f2 = fCount - f1;
    if (m2 != f2) {
        panic("mCount and fCount do not match");
    }

    printf("tests passed :)\n");
}
