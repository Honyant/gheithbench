#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define HEAP_SIZE (1 << 7)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

int main() {

    // assume the program has 32 byte of metadata per free block
    // when a 32 byte block thats leftover
    // it should be marked as free and not grouped into a malloc 
    // even tho size of free block would be 0
    // this way if the block after this empty free block has been freed
    // the two free blocks can coalesce

    void* p1 = malloc(48);
    void* p2 = malloc(16);
    free(p1);
    p1 = malloc(16);
    free(p2);
    p2 = malloc(48);

    long m1 = mCount;
    long f1 = fCount;
  
    if (m1 != 4) {
        printf("m1 %ld\n",m1);
    }
    else if (f1 != 2){
        printf("f1 %ld\n",f1);
    } else {
        printf("count ok!\n");
    }
    printf("pass!\n");
    return 0;
}
