#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define M 20
#define N 100000

#define HEAP_SIZE (1 << 20)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE / sizeof(long)];

void* last[M] = {};

bool isAligned(void* ptr) {
    if ((uintptr_t)ptr % 16 != 0) {
        printf("Pointer not 16 byte aligned\n");
        return false;
    }
    return true;
}

void cleanup() {
    for (int i = 0; i < M; i++) {
        if (last[i] != 0) {
            free(last[i]);
            last[i] = 0;
        }
    }
}

int main() {
    for (uint32_t i = 0; i < M; i++) {
        last[i] = 0;
    }

    // malloc a number larger than heap size
    printf("Test 1: Overflow Mallocation Test\n");
    for (int i = 0; i < 5; i++) {
        // these should return null ptrs
        last[i] = malloc(1L << (24 + i));

        if (last[i] != 0) {
            panic("Returned pointer when allocation is invalid. size: %ld\n", 1L << (24 + i));
        }
    }
    cleanup();
    printf("Test 1 Passed!\n");

    // mallocing 0 should increment mallocs but return null ptrs
    printf("Test 2: Zero Mallocation Test\n");
    for (int i = 0; i < N; i++) {
        // these should return null ptrs
        last[i % M] = malloc(0);

        if (last[i % M] != 0) {
            panic("Returned pointer when allocation is invalid. size: %ld\n", 0);
        }
    }
    cleanup();
    printf("Test 2 Passed!\n");

    // you heap should be completely free at this point
    printf("Test 3: Left Merge Frees\n");
    last[0] = malloc(60);  // should pad to 64
    last[1] = malloc(32);
    last[2] = malloc(100);                                        // should pad to 112
    last[3] = malloc(30);                                         // should pad to 32
    last[4] = malloc(heap_size - 32 * 5 - (64 + 32 + 112 + 32));  // malloc remaining heap

    if (!(isAligned(last[0]) && isAligned(last[1]) && isAligned(last[2]) && isAligned(last[3]))) {
        panic("");
    }

    // check that it's actually been malloced
    for (int i = 0; i < 4; i++) {
        if (last[i] == 0) {
            panic("*** failed to allocate at index: %d\n", i);
        }
    }

    free(last[0]);
    last[0] = 0;
    free(last[1]);
    last[1] = 0;

    last[5] = malloc(92);  // should pad to 96

    if (last[5] == 0) {
        panic("Failing to merge adjacent freed blocks\n");
    }
    cleanup();
    printf("Test 3 passed!\n");

    // you heap should be completely free at this point
    printf("Test 4: Right Merge Frees\n");
    last[0] = malloc(60);  // should pad to 64
    last[1] = malloc(32);
    last[2] = malloc(100);                                        // should pad to 112
    last[3] = malloc(30);                                         // should pad to 32
    last[4] = malloc(heap_size - 32 * 5 - (64 + 32 + 112 + 32));  // malloc remaining heap

    if (!(isAligned(last[0]) && isAligned(last[1]) && isAligned(last[2]) && isAligned(last[3]))) {
        panic("");
    }

    // check that it's actually been malloced
    for (int i = 0; i < 4; i++) {
        if (last[i] == 0) {
            panic("*** failed to allocate at index: %d\n", i);
        }
    }

    free(last[1]);
    last[1] = 0;
    free(last[2]);
    last[2] = 0;

    last[5] = malloc(132);  // should pad to 144

    if (last[5] == 0) {
        panic("Failing to merge adjacent freed blocks\n");
    }
    cleanup();
    printf("Test 4 passed!\n");

    // you heap should be completely free at this point
    printf("Test 5: Left/Right Merge Frees\n");
    last[0] = malloc(60);  // should pad to 64
    last[1] = malloc(32);
    last[2] = malloc(100);                                        // should pad to 112
    last[3] = malloc(30);                                         // should pad to 32
    last[4] = malloc(heap_size - 32 * 5 - (64 + 32 + 112 + 32));  // malloc remaining heap

    if (!(isAligned(last[0]) && isAligned(last[1]) && isAligned(last[2]) && isAligned(last[3]))) {
        panic("");
    }

    // check that it's actually been malloced
    for (int i = 0; i < 4; i++) {
        if (last[i] == 0) {
            panic("*** failed to allocate at index: %d\n", i);
        }
    }

    free(last[0]);
    last[0] = 0;
    free(last[2]);
    last[2] = 0;
    free(last[1]);  // this should merge all three together
    last[1] = 0;

    last[5] = malloc(192);  // should pad to 208

    if (last[5] == 0) {
        panic("Failing to merge adjacent freed blocks\n");
    }
    cleanup();
    printf("Test 5 passed!\n");

    return 0;
}
