

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "heap/heap.h"
#include "heap/panic.h"

//having a small heap size
//checking alignment
//checking malloc and free basic functionalities
//checking metadata stuff and merging is proper

#define HEAP_SIZE (512)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

//helper functions for checks

void PointerStatus(void* p) {
    if (p == NULL) {
        printf("Pointer is null! :(\n");
    }
    else if ((long)p % 16 != 0) {
        printf("Pointer is not 16 byte aligned!\n");
    } else {
        printf("Pointer is not null and well aligned!\n");
    }
}

int main() {

    //mallocing these pointers in order of size
    void* a = malloc(8); //should section out 48 bytes
    void* b = malloc(15); //should section out 48 bytes
    void* c = malloc(30); //should section out 64 bytes
    void* d = malloc(60); //should section out 96 bytes
    void* e = malloc(57); //should section out 96 bytes
    void* f = malloc(93); //should section out 128 bytes

    //this adds up to 480, so keeps 32 bytes of free space depending on heap implementation. flexible

    PointerStatus(a);
    PointerStatus(b);
    PointerStatus(c);
    PointerStatus(d);
    PointerStatus(e);
    PointerStatus(f);

    printf("End of Part A: Malloc and Alignment Functionality!\n");

    //check to see whether things have been malloced and are aligned

    //freeing everything for a consistent malloc and merge check
    
    free(a);
    free(b);
    free(c);
    free(d);
    free(e);
    free(f);

    printf("End of part B: Freeing Functionality!\n");

    //mallocing new pointers with consistent size for an effective malloc after merge test
    //mallocing these pointers in order of size
    //should section out 48 bytes for all of these mallocs
    void* g = malloc(8); 
    void* h = malloc(15); 
    void* i = malloc(8); 
    void* j = malloc(15); 
    void* k = malloc(8); 
    void* l = malloc(15); 
    
    //by pigeonhole principle, need to free 4 things before we get 2 adjancent pointers

    free(h);
    free(j);
    free(k);
    free(l);

    //now have at least 96 bytes of free space (aside from 32 at end for flexible implementation allowance), so can malloc up to 64 bytes

    void* m = malloc(61);

    //checking if it is not null and properly aligned
    PointerStatus(m);

    free(g);
    free(i);
    free(m);

    printf("End of Part C: Merging and Remallocing Functionality!\n");

}
