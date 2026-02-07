#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define HEAP_SIZE (1 << 20)
#define ITERATION_NUM_1 (1 << 10)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];
void *malloced_arr_1[ITERATION_NUM_1];

int main() {
    
    //test 1-3: alignment to 16 when under, exactly, and over 16 bytes modded 
    long *test_1 = (long*) malloc(159);
    long *test_2 = (long*) malloc(160);
    long *test_3 = (long*) malloc(161);

    //check byte alginment
    if (((long) test_1) % 16 == 0) {
        printf("passed alignment 1\n");
    } else {
        printf("incorrect alignment 1\n");
    }

    if (((long) test_2) % 16 == 0) {
        printf("passed alignment 2\n");
    } else {
        printf("incorrect alignment 2\n");
    }

    if (((long) test_3) % 16 == 0) {
        printf("passed alignment 3\n");
    } else {
        printf("incorrect alignment 3\n");
    }

    if (mCount >= fCount) {
        printf("passed tests 1 to 3\n");
    } else {
        printf("failed tests 1 to 3\n");
    }

    free(test_1);
    free(test_2);
    free(test_3);

    //test 4: malloc and free something very small multiple times (stress test on overhead)
    for(int i=0; i<ITERATION_NUM_1; i++) {
        malloced_arr_1[i] = malloc(1);
    }

    //free out of order to check that linked list is working properly
    int start = 0;
    int end = ITERATION_NUM_1 - 1;

    while (start <= end) {
        free(malloced_arr_1[start]);
        if (start != end) {
            free(malloced_arr_1[end]);
        }
        start++;
        end--;
    }

    if(mCount >= fCount ) {
        printf("passed test 4\n"); 
    } else { 
        printf("failed test 4\n"); 
    }
    
    //test 5: change values within the pointer passed to malloc to check correct size
    
    long * ptr = (long*) malloc(201 * sizeof(long));
    
    ptr[0] = 10100; //test with one value; this should get overwritten later

    for(long i=0; i<201; i++) {
        ptr[i] = i * 2; 
    }
    long sum = 0; 
    long expected = 40200;
    for(long i=0; i<201; i++) {
        sum += ptr[i]; 
    }

    if(sum == expected ) {
        printf("passed test 5\n");
    } else {
        printf("failed test 5\n");
    }

    
}
