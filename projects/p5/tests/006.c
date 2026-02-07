#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h" 
#include "heap/panic.h"

#define BLOCK_SIZE 2048
#define HEAP_SIZE ((BLOCK_SIZE*8 + 32) * 3)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];
int success = 0;

void* blocks[3]; // the pointers to allocated blocks
    
void clean_up(){ // free all allocated blocks
    for (int i = 0; i < 3; ++i) {
        if (blocks[i]) free(blocks[i]);
        blocks[i] = NULL;
    }
}
void alloc_all(){ // mallocs all allocated blocks
    for (int i = 0; i < 3; ++i) {
        blocks[i] = malloc(BLOCK_SIZE);
        if (blocks[i] == NULL) panic("*** Failed to allocate block %d\n", i);
    }
}

void test_block_coalescing() {// F = free, U = used
    // this test tests merging of free blocks for a few different cases
    // it actually enforces this because the heap has a limited size here, so it has to merge blocks to fit the new one

    // Test UUU -> UFU (no merging, test basic freeing)
    alloc_all(); 
    free(blocks[1]);
    blocks[1] = malloc(BLOCK_SIZE);
    if (blocks[1] == NULL) panic("*** Failed to allocate for UUU -> FUU\n");
    else success++;
    clean_up();

    // Test UUU -> UUF -> UFF (this should be merged)
    alloc_all();
    free(blocks[2]);
    free(blocks[1]);
    blocks[1] = malloc(BLOCK_SIZE * 2);
    blocks[2] = NULL;
    if (blocks[1] == NULL) panic("*** Failed to allocate for UUU -> UUF -> UFF\n");
    else success++;
    clean_up();

    // Test UUU -> FUU -> FFU (this should be merged)
    alloc_all();
    free(blocks[0]);
    free(blocks[1]);
    blocks[0] = malloc(BLOCK_SIZE * 2);
    blocks[1] = NULL;
    if (blocks[0] == NULL) panic("*** Failed to allocate for UUU-> FUU -> FFU\n");
    else success++;
    clean_up();

    // Test UUU -> FUU -> FUF -> FFF (this should be merged)
    alloc_all();
    free(blocks[0]);
    free(blocks[2]);
    free(blocks[1]);
    blocks[0] = malloc(BLOCK_SIZE * 3);
    blocks[1] = NULL;
    blocks[2] = NULL;
    if (blocks[0] == NULL) panic("*** Failed to allocate for UUU -> FUU -> FUF -> FFF\n");
    else success++;
    clean_up();

    if (success == 4) printf("Block coalescing test passed\n");
    else printf("Block coalescing test failed\n");
}

int main() {
    test_block_coalescing();
    return 0;
}
