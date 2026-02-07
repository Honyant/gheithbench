#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define HEAP_SIZE (1024)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];
// void* last[M] = {};

//Checks if allignment is implemented correctly
void allignment(void* pointer){
    uint32_t* temp = (uint32_t *) pointer;
    if ((uint64_t) temp % 16 != 0 || pointer == 0){
        panic("Allignment is incorrect\n");
    }
}

//Tests general malloc and free, ensures they are alligned correctly
int main() {
    void* p1 = malloc(256);
    void* p2 = malloc(32);
    void* p3 = malloc(128);
    allignment(p3);
    free(p1);
    p1 = malloc(128);
    allignment(p1);
    void* p4 = malloc(32);
    free(p2);
    p2 = malloc(32);
    free(p3);
    free(p4);
    free(p1);
    free(p2);

    long mallocCount = mCount;
    long freeCount = fCount;
  
    if (mallocCount != 6) {
        printf("malloc count not accurate %ld\n", mallocCount);
    } else if (freeCount != 6){
        printf("free count not accurate %ld\n", freeCount);
    } else {
        printf("malloc and free counts valid\n");
    }
    return 0;
}