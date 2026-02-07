#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"


#define HEAP_SIZE (1 << 25)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

void* checkPtr(void* ptr) {
    if (ptr == 0) {
        panic("Pointer is null");
    }
    if ((uint64_t)ptr % 16 != 0) {
        panic("Pointer is not 16 byte aligned");
    }
    return ptr;
}

#define METADATA 32
#define PADDING 32

int main() {

    long m1 = mCount;
    long f1 = fCount;

    // First test: big blocks

    // malloc most of the heap
    void* small = checkPtr(malloc(1234));
    void* huge = checkPtr(malloc(heap_size - 1234 - 654321 - (METADATA*3 + PADDING)));
    void* medium = checkPtr(malloc(654321));

    // Test mallocing both sides of a large block
    free(small);
    free(medium);
    medium = checkPtr(malloc(654321));
    small = checkPtr(malloc(1234));

    // Test mallocing between two other blocks
    free(huge);
    huge = checkPtr(malloc(heap_size - 1234 - 654321 - (METADATA*3 + PADDING)));

    free(small);
    free(medium);
    free(huge);

    long m2 = mCount - m1;
    long f2 = fCount - f1;

    if (m2 != f2 || m2 != 6) {
        panic("Failed first check: m2=%ld, f2=%ld\n", m2, f2);
    }


    // Second test: many small blocks

    // Mallocs almost the whole heap
    int M = 1000;
    int* ptrs[M];
    for (int i = 0; i < M; i++) {
        size_t sz = 64*(i+1);
        ptrs[i] = checkPtr(malloc(sz));

        *ptrs[i] = i;
    }

    // Free and reallocate randomly
    long randomMallocs = 0;
    int N = 20000000;
    for (int i = 0; i < N; i++) {
        int idx = rand() % M;
        if (ptrs[idx] == 0) {
            ptrs[idx] = checkPtr(malloc(64));
            *ptrs[idx] = idx;
            randomMallocs++;
        } else {
            if (*ptrs[idx] != idx) {
                panic("Value overwritten: %d!\n", idx);
            }

            free(ptrs[idx]);
            ptrs[idx] = 0;
        }
    }

    // Free all
    for (int i = 0; i < M; i++) {
        if (ptrs[i] != 0) {
            free(ptrs[i]);
        }
    }

    long m3 = mCount - m1 - m2;
    long f3 = fCount - m1 - f2;
    if (m3 != f3 || m3 != M + randomMallocs) {
        panic("Failed second check: m3=%ld, f3=%ld\n", m3, f3);
    }

    // malloc the whole heap at the end to test fragmentation
    huge = checkPtr(malloc(heap_size - (METADATA + PADDING)));
    free(huge);

    long m4 = mCount - m1 - m2 - m3;
    long f4 = fCount - f1 - f2 - f3;
    if (m4 != f4 || m4 != 1) {
        panic("Failed third check: m4=%ld, f4=%ld\n", m4, f4);
    }

    printf("End of test :)\n");

}
