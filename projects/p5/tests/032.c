#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define HEAP_SIZE (1 << 10)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];


int main() {

    // TEST 1: Too big malloc

    void* p = malloc(heap_size + sizeof(long));
    if (p != NULL && p != 0) {
        printf("TEST 1: ERROR. Malloced when there wasn't enough space.\n");
    } else {
        printf("TEST 1: SUCCESS. Did not malloc when block too large.\n");
        free(p); // implicitly testing free
    }

    // TEST 2: Merging
        // make 3 blocks that fill up the space
        // free them
        // add smth that requires almost all space and therefore a merge

    void* ptrs[3];
    for (int i = 0; i < 3; i++) {
        ptrs[i] = malloc(128);
    }
    for (int i = 0; i < 3; i++) {
        free(ptrs[i]);
    }
    p = malloc(400); // this will require space from 3 merged blocks
    if (p != NULL && p != 0) {
        printf("TEST 2: SUCCESS. Merged blocks and made space for bigger malloc.\n");
        free(p);
    } else {
        printf("TEST 2: FAILED. Did not merge blocks to made space for bigger malloc.\n");
    }
    
    // TEST 3: 16 Alignment
        // make random blocks not % by 16 and check if pointer is % 16
    
    void* ptrs2[10];
    char pass = 1;

    for (int i = 0; i < 10; i++) {
        long r = 0;
        while (r % 16 == 0) {
            r = rand() % 80; // generate random numbers up to 80 and not % by 16
        }
        ptrs2[i] = malloc(r);
        if (ptrs2[i] != NULL && ptrs2[i] != 0) {
            // check if ptr is %16
            if ((long) ptrs2[i] % 16 != 0) {
                pass = 0;
            }
            
        }
    }

    if (pass) {
        printf("TEST 3: SUCCESS. All pointers are 16-aligned.\n");
    } else {
        printf("TEST 3: FAILED. Not all pointers are 16-aligned.\n");
    }

    for (int i = 0; i < 10; i++) {
        if (ptrs2[i] != NULL && ptrs2[i] != 0) {
            free(ptrs2[i]);
        }
    }

    // TEST 4: Freeing in Reverse Order
    
    void* ptrs3[10];

    long m1 = mCount;
    long f1 = fCount;

    for (int i = 0; i < 10; i++) {
        long r = rand() % 80; // generate random numbers up to 80
        ptrs3[i] = malloc(r);
    }
    for (int i = 9; i >= 0; i--) {
        if (ptrs3[i] != NULL && ptrs3[i] != 0) {
            free(ptrs3[i]);
        }
    }

    long m2 = mCount - m1;
    long f2 = fCount - f1;

    if (m2 == f2) {
        printf("TEST 4: SUCCESS. Reverse freeing done successfully.\n");
    } else {
        printf("TEST 4: FAILED. Reverse freeing resulted in an uneven number of mallocs/frees.\n");
    }

    // TEST 5: Freeing in Random Order
    
    void* ptrs4[10];

    m1 = mCount;
    f1 = fCount;

    int freeOrder[10] = {7,2,9,6,8,4,1,5,3};

    for (int i = 0; i < 10; i++) {
        long r = rand() % 80; // generate random numbers up to 80
        ptrs4[i] = malloc(r);
    }

    for (int i = 0; i < 10; i++) {
        int index = freeOrder[i]; // free in order specified by freeOrder arr
        if (ptrs4[index] != NULL && ptrs4[index] != 0) {
            free(ptrs4[index]);
        }
    }

    m2 = mCount - m1;
    f2 = fCount - f1;

    if (m2 == f2) {
        printf("TEST 5: SUCCESS. Random freeing done successfully.\n");
    } else {
        printf("TEST 5: FAILED. Random freeing resulted in an uneven number of mallocs/frees.\n");
    }

    return 0;
}
