#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define HEAP_SIZE (1 << 20)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

void* malloc_and_check(int size) {
    long* p = malloc(size);
    if(p == 0) 
        panic("failed to allocate %ld bytes :(\n", size);
    else if((long)p % 16 != 0) 
        panic("pointer address %lx is not 16-byte aligned :(\n", (long)p);
    return p;
}

int main() {
    // test 1: check merging
    long size = (HEAP_SIZE - 70) / 2;
    long* p1 = malloc_and_check(size);
    long* p2 = malloc_and_check(size);
    free(p1);
    free(p2);
    long* p3 = malloc_and_check(HEAP_SIZE - 70);
    free(p3);
    printf("Passed test 1 :D\n");

    // test 2: make sure enough space allocated
    long* p4 = malloc_and_check(5 * sizeof(long));
    for(int i = 0; i < 6; i++)
        p4[i] = i;
    printf("Passed test 2 :D\n");

    // test 2: check byte alignment
    long* p5 = malloc_and_check(5);
    long* p6 = malloc_and_check(15);
    printf("Passed test 3 :D\n");

    // test 4: random mallocs and frees
    int n = 10000;
    long* ptrs[n];
    for(int i = 0; i < n; i++) {
        size_t size = (size_t)(rand() % 100 + 1);
        ptrs[i] = malloc_and_check(size);

        if((i+1) % 10 == 0) {
            for(int j = i; j > i-10; j--) {
                if(rand() % 2 == 0) {
                    free(ptrs[j]);
                    ptrs[j] = 0;
                }
            }
        }
    }
    for(int i = 0; i < n; i++) {
        if(ptrs[i] != 0) free(ptrs[i]);
    }
    printf("Passed test 4 :D\n");

    // test 5: check that values weren't overriden
    for(int i = 0; i < 5; i++) {
        if(p4[i] != i) 
            panic("Value should have been %d, but was %d :(\n", i, p6[i]);
    }
    free(p6);
    free(p5);
    free(p4);
    printf("Passed test 5 :D\n");
    printf("Yay!\n");

    return 0;
}