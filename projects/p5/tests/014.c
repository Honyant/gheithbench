#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define HEAP_SIZE 4096

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

int main() {
    // Test 1: check alignment to 16 bytes
    int count1 = 16;
    char *ptrs1[count1];
    // iterate through all remainders of 16
    for (int i = 0; i < count1; i++) {
        size_t size = 15 * (i + 1);
        ptrs1[i] = malloc(size);
        if ((uintptr_t) ptrs1[i] % 16 != 0) {
            panic("Test 1: fail --- block of size %ld not aligned\n", (uintptr_t) ptrs1[i]);
        }
    }
    printf("Test 1: pass\n");

    // Test 2: check modifying and preserving values
    int count2 = 20;
    int values[count2];
    char *ptrs2[count2];
    size_t size2 = 16;
    for (int i = 0; i < count2; i++) {
        ptrs2[i] = malloc(size2);
        values[i] = rand() % 100;
        // assign value at beginning of block and value + 1 at end of block
        *ptrs2[i] = values[i];
        *(ptrs2[i] + size2 - 1) = values[i] + 1;
    }
    for (int i = 0; i < count2; i++) {
        if (*ptrs2[i] != values[i])
            panic("Test 2: fail --- value %d at 0x%lx not preserved\n", values[i], (uintptr_t) ptrs2[i]);
        if (*(ptrs2[i] + size2 - 1) != values[i] + 1) {
            panic("Test 2: fail --- value %d at 0x%lx not preserved\n", values[i] + 1, (uintptr_t) ptrs2[i] + size2 - 1);
        }
    }
    printf("Test 2: pass\n");

    // Test 3: check merging after freeing blocks
    for (int i = 0; i < count1; i++) {
        free(ptrs1[i]);
    }
    for (int i = 0; i < count2; i++) {
        free(ptrs2[i]);
    }
    // there will only be space if free blocks are merged
    size_t size3 = 4000;
    char *p = malloc(size3);
    if (p == NULL) {
        panic("Test 3: fail --- failed to allocate block size %ld\n", size3);
    }
    free(p);
    printf("Test 3: pass\n");

    // Test 4: test malloc and immediate free
    int count4 = 1000;
    for (int i = 0; i < count4; i++) {
        size_t size = rand() % 500 + 1;
        long *p = malloc(size);
        if (p == NULL) {
            panic("Test 4: fail --- failed to allocate block size %ld\n", size);
        }
        free(p);
    }
    printf("Test 4: pass\n");

    // Test 5: test freeing in spaced intervals
    int count5 = 40;
    char *ptrs5[count5];
    for (int i = 0; i < count5; i++) {
        size_t size = rand() * 64 + 1;
        ptrs5[i] = malloc(size);
    }
    // stagger freeing blocks
    for (int i = 0; i < count5; i += 4) {
        free(ptrs5[i]);
    }
    for (int i = 1; i < count5; i += 4) {
        free(ptrs5[i]);
    }
    for (int i = 2; i < count5; i += 4) {
        free(ptrs5[i]);
    }
    for (int i = 3; i < count5; i += 4) {
        free(ptrs5[i]);
    }
    printf("Test 5: pass\n");

    // Test 6: test freeing in random order
    int count6 = 40;
    char *ptrs6[count6];
    for (int i = 0; i < count6; i++) {
        size_t size = rand() * 64 + 1;
        ptrs6[i] = malloc(size);
    }
    int free_order[count6];
    for (int i = 0; i < count6; i++) {
        free_order[i] = i;
    }
    // generate random order to free blocks
    for (int i = 0; i < count6 - 1; i++) {
        int swap = rand() % (count6 - i - 1) + i + 1;
        int temp = free_order[i];
        free_order[i] = free_order[swap];
        free_order[swap] = temp;
    }
    // free in random order
    for (int i = 0; i < count6; i++) {
        free(ptrs6[free_order[i]]);
    }
    printf("Test 6: pass\n");

    return 0;
}
