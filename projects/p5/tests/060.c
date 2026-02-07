#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define HEAP_SIZE (1 << 20)
//initialize a large heap
long heap_size = HEAP_SIZE;

int main() {
    //pointer basic alloc and dealloc
    void* a = malloc(1);
    if (a == 0) {
        panic("malloc failed\n");
    }
    if ((uintptr_t)a % 16 != 0) {
        panic("padding failed\n");
    }
    free(a);
    void* b = malloc(1);
    void* c = malloc(1);
    if (c == 0) {
        panic("malloc failed\n");
    }
    if ((uintptr_t)c % 16 != 0) {
        panic("padding failed\n");
    }
    free(b);
    free(c);
    //pointer test allocating a big memory space and then allocating more memory space
    //assume that padding isnt super ridiulous
    void* d = malloc(heap_size/2);
    void* e = malloc(1);
    if (e == 0) {
        panic("malloc failed\n");
    }
    if ((uintptr_t)e % 16 != 0) {
        panic("padding failed\n");
    }
    free(d);
    free(e);
    //mallocing a bunch of stuff, and seeing if it would mess up
    long* arr[1000];
    for(int i = 0;i<1000;i++)
    {
        arr[i] = malloc(16);
        if (arr[i] == 0) {
        panic("malloc failed\n");
    }
    }
    for(int i=0;i<1000;i++)
    {
        free(arr[i]);
    }
    printf("success\n");
    return 0;
}