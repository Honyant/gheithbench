#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define HEAP_SIZE 4096

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

void* p1;
void* p2;
void* p3;
long* p4;
long* p5;

long* arr[100];

int main() {

    //test 1
    //basic malloc and free test
    p1 = malloc(18);
    p2 = malloc(72);
    p3 = malloc (1 << 10);
    free(p1);
    free(p2);
    free(p3);
    printf("passed test 1\n");

    //test 2
    //16-bit alignment test
    p1 = malloc(17);
    p2 = malloc(45);
    p3 = malloc((1 << 9) + 13);
    p4 = malloc(743);
    if ((long)p1 % 16 != 0) {
        panic("ERROR, p1 not aligned\n");
    }
    if ((long)p2 % 16 != 0) {
        panic("ERROR, p2 not aligned\n");
    }
    if ((long)p3 % 16 != 0) {
        panic("ERROR, p3 not aligned\n");
    }
    if ((long)p4 % 16 != 0) {
        panic("ERROR, p4 not aligned\n");
    }
    printf("passed test 2\n");

    //test 3
    //coalescing test: you should have enough room to malloc all of these pointers
    //this test assumes at most 32 bytes of metadata per block of memory
    p1 = malloc(2016);
    p2 = malloc(2016);
    free(p1);
    free(p2);
    p3 = malloc(4064);
    free(p3);
    printf("passed test 3\n");


    //test 4
    //preserve values tests
    p4 = malloc(2016);
    *p4 = 1045;
    p5 = malloc(2016);
    if (*p4 != 1045) {
        panic("ERROR, pointer value not preserved for test 3");
    }
    printf("passed test 4\n");

    //test 5
    //large-scale malloc and free
    for (int i = 0; i < 100; i++) {
        arr[i] = malloc(i + 1);
        if (i % 2 == 1) {
            free(arr[i - 1]);
        }
    }
    printf("passed test 5\n");


}
