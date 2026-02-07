#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define HEAP_SIZE (8192 - 16)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];


int main() {
    //there will always be places to allocate these
    long* pX = malloc(4096);
    long* pS1 = malloc(1024);
    long* pS2 = malloc(1024);
    for (int i = 0; i < 2048/sizeof(long); i++) {
        pX[i] = i + 1000;
    }
    for (int i = 0; i < 1024/sizeof(long); i++) {
        pS1[i] = i + 2000;
    }
    for (int i = 0; i < 1024/sizeof(long); i++) {
        pS2[i] = i + 3000;
    }
    //make sure they aren't bleeding into each other
    for (int i = 0; i < 2048/sizeof(long); i++) {
        if (pX[i] != i + 1000) {
            panic("ERR: memory overlowed from another block into block of size 2048");
        }
    }
    for (int i = 0; i < 1024/sizeof(long); i++) {
        if (pS1[i] != i + 2000) {
            panic("ERR: memory overlowed from another block into block of size 1024");
        }
    }
    for (int i = 0; i < 1024/sizeof(long); i++) {
        if (pS2[i] != i + 3000) {
            panic("ERR: memory overlowed from another block into block of size 1024");
        }
    }
    if (mCount != 3) {
        panic("mcount is wrong... it should be 3");
    }
    free(pX);
    
    //test fragmentation
    long** pointerArr[50];
    for (int i = 0; i < 50; i++) {
        long** temp = malloc(1024);
        pointerArr[i] = malloc(65);
        if (i >= 25) {
            free(pointerArr[i-25]);
        }
        free(temp);
    }
    for (int i = 25; i < 50; i++) {
        free(pointerArr[i]);
    }
    if (fCount != 101) {
        panic("freeCount should be 101... instead it is %d", fCount);
    }
    printf("success!\n");
    return 0;
}