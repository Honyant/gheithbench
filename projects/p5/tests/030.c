#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define HEAP_SIZE (1 << 20)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

int main() {
    //malloc a big chunk, free, then remalloc
    void* ptr = malloc(100000);
    if (ptr == 0) panic("failed to malloc first block\n");
    free(ptr);
    ptr = malloc(10394);
    if (ptr == 0) panic("failed to remalloc first block\n");
    free(ptr);
    printf("passed basic malloc, free, and remalloc\n");

    //test that mallocs are 16-byte aligned
    void* ptrs[100] = {};
    for (int i = 0; i < 100; i++) {
        size_t mallocSize = rand() % 300 + 1;
        ptrs[i] = malloc(mallocSize);
        if ((long) ptrs[i] % 16 != 0) 
            panic("pointer %ld is not 16-byte aligned\n", (long) ptrs[i]);
        if ((long) ptrs[i] == 0)
            panic("failed to malloc %ld bytes\n", mallocSize);
    }
    printf("passed 16-byte alignment\n");

    //free everything in random order
    for (int i = 0; i < 100; i++) {
        int idx = rand() % 100;
        if ((long) ptrs[idx] != -1) {
            free(ptrs[idx]);
            ptrs[idx] = (void*) -1;
        }
    }
    for (int i = 0; i < 100; i++) {
        if ((long) ptrs[i] != -1) free(ptrs[i]);
    }
    printf("passed random malloc/frees\n");

    //test different cases for freeing/coalescing
    void* block1 = malloc(16);
    void* block2 = malloc(32);
    void* block3 = malloc(351);
    void* block4 = malloc(23);
    void* block5 = malloc(452);
    void* block6 = malloc(34);
    free(block2); //no coalescing
    free(block4); //no coalescing
    free(block3); //coalesce on both sides
    free(block1); //right coalesce
    free(block5); //left coalesce
    free(block6);
    block1 = malloc(heap_size / 2 - 10000);
    block2 = malloc(heap_size / 2 - 10000);
    if (block1 == 0) panic("failed to malloc %ld bytes\n", heap_size/2-10000);
    if (block2 == 0) panic("failed to malloc %ld bytes\n", heap_size/2-10000);
    free(block2);
    free(block1);
    block3 = malloc(heap_size - 10000);
    if (block3 == 0) panic("did not coalesce\n");
    free(block3);
    printf("passed different variations of coalescing\n");

    return 0;
}
