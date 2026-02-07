#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define M 1000
#define N 100000

#define HEAP_SIZE (1 << 24)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

char* pointers[3 * N] = {};
char* p1;
char* p2;
char* p3;
char* p4;

// makes sure pointers are 16-byte aligned
bool isByteAligned(void * pointer){
    if(((uint64_t) pointer) % 16 != 0){
        return false;
    }
    return true;
}

int main() {

    for (int i = 0; i < 3 * N; i++) {
        pointers[i] = 0;
    }

    long m1 = mCount;
    long f1 = fCount;
    int testNum = 1;

    // test 1: mallocing different sizes then instantly freeing
    for (int i = 1; i <= N; i++) {
        p1 = (char*) malloc(i);
        if (!p1) {
            printf("Test %d failed\n", testNum);
            panic("*** Allocation failed of size: %d\n", i);
        }
        if (!isByteAligned(p1)) {
            printf("Test %d failed\n", testNum);
            panic("*** Pointer is not 16-byte aligned: %ld\n", (uint64_t) p1 % 16);
        }

        // ensuring values pointed to by pointer can be modified
        p1[0] = 5;
        p1[0] = 10;
        p1[i - 1] = 'b';
        if ((i == 1 && p1[0] != 'b') || (i != 1 && p1[0] + p1[i - 1] != 'l')) {
            printf("Test %d failed\n", testNum);
            panic("*** Values pointed to by the pointer were not updated properly\n");
        }
        free(p1);
    }

    if (mCount - m1 != N || fCount - f1 != N) {
        printf("Test %d failed\n", testNum);
        printf("Number of mallocs or number of frees were not equal to target value of %d\n", N);
    } else {
        printf("Test %d passed!\n", testNum);
    }

    testNum++;
    // test 2: Large malloc allocations (will only work if merging is implemented correctly)
    m1 = mCount;
    f1 = fCount;

    p1 = (char*) malloc(1 << 22);
    p2 = (char*) malloc(1 << 22);
    p3 = (char*) malloc(1 << 22);
    p4 = (char*) malloc(1 << 21);

    if (!p1 || !p2 || !p3 || !p4) {
        printf("Test %d failed\n", testNum);
        panic("*** Massive allocation sizes failed\n");
    }

    if (!isByteAligned(p1) || !isByteAligned(p2) || !isByteAligned(p3) || !isByteAligned(p4)) {
        printf("Test %d failed\n", testNum);
        panic("*** Pointers are not 16-byte aligned\n");
    }

    free(p4);
    free(p2);
    free(p3);
    free(p1);

    p1 = malloc(1 << 23); // the 4 freed blocks above must have been merged for this to work
    if (!p1) {
        printf("Test %d failed\n", testNum);
        panic("*** Merging does not work\n");
    }
    if (!isByteAligned(p1)) {
        printf("Test %d failed\n", testNum);
        panic("*** Pointer is not 16-byte aligned: %ld\n", (uint64_t) p1 % 16);
    }

    free(p1);
    if (mCount - m1 != 5 || fCount - f1 != 5) {
        printf("Test %d failed\n", testNum);
        printf("Number of mallocs or number of frees were not equal to target value of %d\n", 5);
    } else {
        printf("Test %d passed!\n", testNum);
    }

    testNum++;
    // test 3: mallocing all at once, then freeing alternating pointers (to mess around with merging cases)
    m1 = mCount;
    f1 = fCount;
    for (int i = 1; i < M; i++) {
        pointers[i] =(char*) malloc(i);
        if (!pointers[i]) {
            printf("Test %d failed\n", testNum);
            panic("*** Allocation failed of size: %d\n", i);
        }
        if (!isByteAligned(pointers[i])) {
            printf("Test %d failed\n", testNum);
            panic("*** Pointer is not 16-byte aligned: %ld\n", (uint64_t) pointers[i] % 16);
        }
    }

    // freeing all odd pointers
    for (int i = 1; i < M; i += 2) {
        free(pointers[i]);
    }

    // freeing all even pointers
    for (int i = 2; i < M; i += 2) {
        free(pointers[i]);
    }

    if (mCount - m1 != M - 1 || fCount - f1 != M - 1) {
        printf("Test %d failed\n", testNum);
        printf("Number of mallocs or number of frees were not equal to target value of %d\n", M - 1);
    } else {
        printf("Test %d passed!\n", testNum);
    }

    testNum++;
    // test 4: Ensuring a reasonable amount of meta data is used (32 bytes of metadata will pass) + ensuring a free list is created so that the full array isn't traversed while mallocing
    m1 = mCount;
    f1 = fCount;
    for (int i = 0; i < 3 * N; i++) {
        pointers[i] = (char*) malloc(1);
        if (!pointers[i]) {
            printf("Test %d failed\n", testNum);
            panic("*** Allocation failed of size: %d\n", 1);
        }
        if (!isByteAligned(pointers[i])) {
            printf("Test %d failed\n", testNum);
            panic("*** Pointer is not 16-byte aligned: %ld\n", (uint64_t) pointers[i] % 16);
        }
    }
    for (int i = 0; i < 3 * N; i++) {
        free(pointers[i]);
    }
    
    if (mCount - m1 != 3 * N || fCount - f1 != 3 * N) {
        printf("Test %d failed\n", testNum);
        printf("Number of mallocs or number of frees were not equal to target value of %d\n", 3 * N);
    } else {
        printf("Test %d passed!\n", testNum);
    }

    return 0;
}
