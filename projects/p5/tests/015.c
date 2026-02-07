#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"


#define HEAP_SIZE (1 << 25) //we can do this with a smaller heap
#define N 500000

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

char* str_ptrs[5000] = {};

int main() {
    //first stress test for large memory sizes, make sure you can malloc and free them, and still do it after merges
    for(int i = 0; i < 1000; i++){
        int sub_size = 96 + rand()%1000;
        long* large = malloc(HEAP_SIZE - sub_size);
        if(large == NULL){
            panic("failed to malloc block of size %d", HEAP_SIZE - sub_size);
        }
        free(large);
    }
    
    //now if we malloc things in the middle, we check that they are merged when we free
    long* ptr1 = (long*)malloc(123);
    long* ptr2 = (long*)malloc(456);
    long* ptr3 = (long*)malloc(789);

    //free in a different order than we malloced to test merges, should test both a left and right merge
    free(ptr2);
    free(ptr1);
    free(ptr3);

    long* ptr4 = (long*)malloc(HEAP_SIZE - 64);
    if(ptr4 == NULL){
        panic("Attempted to allocate large block after all memory has been freed but failed \n");
    }
    free(ptr4);
    printf("Passed Case 1!\n");

    //simple second test, checks to see whether you are mallocing a freeing correctly with alignment


    for(int i = 1; i < 5000; i++) {
        char* ptr = (char*) malloc(i);
        ptr[0] = 120;
        if(ptr[0] != 120){ //make sure we can overwrite
            printf("can't access value \n");
        }
        ptr[i-1]=100;
        if(ptr[i-1] != 100){
            printf("can't access value \n");
        }
        str_ptrs[i] = ptr; 
    }
    
    for(int i = 1; i < 5000; i++) {
        if((long)str_ptrs[i] % 16 != 0){
            printf("alignment wrong \n");
        }
        free(str_ptrs[i]);
    }

    printf("Passed Case 2!\n");

    //third test - simple stress test to test that you are using a free list, will fail without
    long* ptrs [N] = {};
    for(int i = 0; i < N; i++){
        long* mallocOne = (long*) malloc(1);
        if(mallocOne == NULL || mallocOne == 0){
            panic("Attempted to malloc 1, but null pointer was returned");
        }
        ptrs[i] = mallocOne;
    }
    for(int i = 0; i < N; i++){
        free(ptrs[i]);
    }
    
    printf("Passed Case 3!\n");

    //fourth test - these are a few edge case tests for pointers
    long* stress_test_zeroes[10000] = {};
    for(int i = 0; i < 10000; i++){
        stress_test_zeroes[i] = (long*) malloc(0);
    }
    for(int i = 0; i < 10000; i++){
        free(stress_test_zeroes[i]);
    }

    //test freeing null pointers
    for(int i = 0; i < 10000; i++){
        free(NULL);
    }

    printf("Passed Case 4!\n");
    
    printf("All Done!\n");

    return 0;
}
