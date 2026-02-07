#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define HEAP_SIZE (1 << 20)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

char* ptrs[100] = {};

int main() {
    long m1 = mCount;
    long f1 = fCount;
    

    // mass malloc and free
    for(int i = 1; i < 100; i++) {
        char* ptr = (char*) malloc(i);
        ptr[0] = 69;
        ptr[i-1]=13;
        ptrs[i] = ptr; 
    }
    for(int i = 1; i < 100; i++) {
        free(ptrs[i]);
    }

    // let's do it again

    // mass malloc and free
    for(int i = 1; i < 100; i++) {
        char* ptr = (char*) malloc(i);
        ptr[0] = 0b10101010;
        ptr[i-1]=0b11111111;
        ptrs[i] = ptr;
    }
    for(int i = 1; i < 99; i++) { // we leave one block at the end, so a large gap is formed
        free(ptrs[i]);
    }

    // malloc in gap
    for(int i = 1; i < 99; i++) {
        char* ptr = (char*) malloc(i);
        ptr[0] = 0b11111111;
        ptr[i-1]=69;
        ptrs[i] = ptr;
    }

    // free it all
    for(int i = 1; i < 100; i++) {
        free(ptrs[i]);
    }


    // end checks
    long m2 = mCount - m1;
    long f2 = fCount - f1;
  
    if (m2 != f2) {
        printf("m2 %ld\n",m2);
        printf("f2 %ld\n",f2);
    } else {
        printf("count match\n");
    }

    if (m2 != 296) {
        printf("*** wrong count %ld\n",m2);
    } else {
        printf("count ok\n");
    }


    return 0;
}
