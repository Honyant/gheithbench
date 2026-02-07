#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define MAX_METADATA_SIZE 32

#define HEAP_SIZE (1 << 18)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

void check_alignment(void *ptr) {
    bool aligned = (int64_t) ptr % 16 == 0;
    if (!aligned) {
        panic("Pointer is not 16-byte aligned.\n");
    }
}

// Test:
// - Edge case of NULL/malloc(0)
// - Pointers are 16-byte aligned 
void one_pointer_tests() {
    free(NULL); 

    void *ptr = malloc(0);
    free(ptr);

    ptr = malloc(16);
    check_alignment(ptr);
    free(ptr);

    ptr = malloc(23);
    check_alignment(ptr);
    free(ptr);

    ptr = malloc(17);
    check_alignment(ptr);
    free(ptr);
}

// Test: 
// - Allocates entire heap in uniform blocks
// - Ensures that blocks are not overlapping because they store unique values 
void overlapping_test() {
    int block_size = 128;

    void *block[block_size]; // Array of void ptrs


    // Fill the array
    for (size_t i = 0; i < block_size; i++) {
        // Save 32 bytes at start of heap
        size_t limit = i == 0 ? 2048 - 2 * MAX_METADATA_SIZE : 2048 - MAX_METADATA_SIZE;

        block[i] = malloc(limit);
        check_alignment(block[i]);
        if (block[i] == NULL) {
            panic("Allocating block %d of %d failed. \n", i, block_size);
        }

        // Set all of the memory in this block to a unique value 
        unsigned char *block_ptr = block[i];
        for (size_t j = 0; j < limit; j++) {
            block_ptr[j] = i;
        }
    }

    // Free the array
    for (size_t i = 0; i < block_size; i++) {
        // Save 32 bytes at start of heap
        size_t limit = i == 0 ? 2048 - 2 * MAX_METADATA_SIZE : 2048 - MAX_METADATA_SIZE;

        unsigned char *block_ptr = block[i];

        for (size_t j = 0; j < limit; j++) {
            if (block_ptr[j] != i) {
                panic("Block %i's memory is no longer the same.\n", i);
            }
        }
        free(block_ptr);
    }
}

// Test:
// - Allocating to the entire heap
void full_heap_test() {
    void* full_heap = malloc(HEAP_SIZE - 2 * MAX_METADATA_SIZE);
    if (full_heap == NULL) {
        panic("Allocating entire heap after freeing data failed.\n");
    }
    free(full_heap);
}

// Test:
// - Tests Merging Scenarios
void test_merge() {
    void *p1 = malloc(16);
    free(p1);
    full_heap_test();

    p1 = malloc(16);
    void *p2 = malloc(16);
    void *p3 = malloc(16);
    free(p1);
    free(p2);
    free(p3);
    full_heap_test();

    p1 = malloc(16);
    p2 = malloc(16);
    p3 = malloc(16);
    free(p3);
    free(p2);
    free(p1);
    full_heap_test();

    p1 = malloc(16);
    p2 = malloc(16);
    p3 = malloc(16);
    free(p2);
    free(p1);
    free(p3);
    full_heap_test();

    p1 = malloc(16);
    p2 = malloc(16);
    p3 = malloc(16);
    free(p2);
    free(p3);
    free(p1);
    full_heap_test();
}

int main() {
    // Run tests in a loop to make sure memory is being reused correctly
    for (int i = 0; i < 2; i++) {
        one_pointer_tests();
        test_merge();
        overlapping_test();
    }

    printf("All test cases passed!\n");

    return 0; // test case success
}
