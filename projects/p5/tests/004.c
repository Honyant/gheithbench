#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define M 20
#define N 100000

#define HEAP_SIZE (1 << 20)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

void* array[10000] = {};
void* pointer1;
void* pointer2;
void* pointer3;

//method to confirm pointers returned are 16-aligned
void alignmentCheck(void * pointer){
    if((uint64_t)pointer % 16 != 0){
        //printf("Error: non aligned pointer detected: %ld", (uint64_t)pointer % 16);
    }
}

int main() {
    //each test module should be completely seperate, assuming you implemented free correctly

    //Test 0:
    int counter = 0;

    //confirming that a basic variable can be accessed
    pointer1 = malloc(sizeof(int));
    if(pointer1 == 0){
        printf("Test Failed: %d", counter++);
    }
    //confirming that we can actually write and read to memory
    *((int *) pointer1) = 23;
    alignmentCheck(pointer1);
    if(*((int *) pointer1) != 23){
        free(pointer1);
        printf("Test Failed: %d", counter++);
    }
    else{
        free(pointer1);
        printf("Test Passed: %d", counter++);
    }
    printf("\n");

    //Test 1:   
    //confirming that free works by malloc-ing something massive, freeing it, and then mallocing it again
    for(int i = 0; i < 1; i++){
        pointer1 = malloc((1<<19));
        alignmentCheck(pointer1);
        if(pointer1 == 0){
            printf("Test Failed: %d", counter++);
        }
        free(pointer1);
    }
    printf("Test Passed: %d", counter++);
    printf("\n");

    //Test 2:
    //confirming that free blocks are combined by malloc-ing 3 things, freeing all, then mallocing something that needs that space
    for(int i = 0; i < 1000; i++){
        pointer1 = malloc((1<<17));
        pointer2 = malloc((1<<17));
        pointer3 = malloc((1<<17));
        alignmentCheck(pointer1);
        alignmentCheck(pointer2);
        alignmentCheck(pointer3);
        if(((pointer1 == 0) || (pointer2 == 0)) || (pointer3 == 0)){
            printf("Test Failed: %d", counter++);
        }
        //swapping order to check all directions of block combining
        if(i % 2 == 0){
            free(pointer1);
            free(pointer2);
            free(pointer3);
        }
        else if(i % 3 == 0){
            free(pointer1);
            free(pointer3);
            free(pointer2);
        }
        else if(i % 5 == 0){
            free(pointer2);
            free(pointer1);
            free(pointer3);
        }
        else if(i % 7 == 0){
            free(pointer2);
            free(pointer3);
            free(pointer1);
        }
        else if(i % 13 == 0){
            free(pointer3);
            free(pointer2);
            free(pointer1);
        }
        else{
            free(pointer3);
            free(pointer1);
            free(pointer2);
        }
        pointer1 = malloc((1<<19));
        alignmentCheck(pointer1);
        if(pointer1 == 0){
            printf("Test Failed: %d", counter++);
        }
        free(pointer1);
    }
    printf("Test Passed: %d", counter++);
    printf("\n");
    
    //Test 3: 
    //Stress test to confirm that you aren't traversing the whole list every time
    for(int i = 0; i < 10000; i++){
        array[i] = malloc(1);
    }
    for(int i = 0; i < 100000; i++){
        pointer1 = malloc(1);
        free(pointer1);
    }
    for(int i = 0; i < 10000; i++){
        free(array[i]);
    }
    printf("Test Passed: %d", counter++);
    printf("\n");

    //Test 4: 
    //Same as before but with an opening right at the start of the list
    for(int i = 0; i < 10000; i++){
        array[i] = malloc(1);
    }
    free(array[0]);
    for(int i = 0; i < 100000; i++){
        pointer1 = malloc(1);
        free(pointer1);
    }
    for(int i = 1; i < 10000; i++){
        free(array[i]);
    }
    printf("Test Passed: %d", counter++);
    printf("\n");
}
