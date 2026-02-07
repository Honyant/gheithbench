#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"


#define M 20
#define N 100000

#define HEAP_SIZE 8192

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

int main(){

    long bytes=4095;
    void * ptrs[10];

    int isAligned=1;
    int fullHeap=1;
    int uniquePointers=1;

    ptrs[0]=0;

    ptrs[0]=ptrs[0];

    //does some basic mallocing and checks alignment

    for (int i=0;i<10;i++){
        ptrs[i]=malloc(bytes);
        bytes=(bytes-1)/2;
    }

    for (int i=0;i<10;i++){
        if((long)ptrs[i] %16!=0){
            isAligned=0;
        }
    }

    // at this point the heap should be full so every malloc should return NUll now

    void* temp;

    for(int i=0;i<100;i++){
        temp = malloc(500);
        if (temp!=NULL){
            fullHeap=0;
        }
    }

    for (int i=0;i<10;i++){
        free(ptrs[i]);
    }

    //test mallocing even more random values

    void * morePtrs[100];

    for (int i=0;i<100;i++){
        morePtrs[i]=malloc(rand()%20 +1);
    }

    //check that every malloc returns a unique pointer

    for (int i=0;i<100;i++){
        for (int j=i+1;j<100;j++){
            if (morePtrs[i]==morePtrs[j]){
                uniquePointers=0;
            }
        }
    }

    //free the indices in a random order

    int freeIndices[100];
    for (int i=0;i<100;i++){
        freeIndices[i]=i;
    }

    temp = malloc(0);
    
    int freeIndex;
    int random;
    int swap=0;
    for (int i=99;i>=1;i--){
        random = (int)(rand()%i);
        freeIndex=freeIndices[random];
        swap = freeIndices[i];
        freeIndices[i]=freeIndices[random];
        freeIndices[random]=swap;
        free(morePtrs[freeIndex]);
    }
    free(morePtrs[freeIndices[0]]);

    temp = malloc(0);



    if(mCount!=212){
        printf("Your counting of malloc is incorrect\n");
    }
    if(fCount!=110){
        printf("Your counting of freeing is incorrect\n");
    }
    if(!isAligned){
        printf("Your pointers are not 16 byte aligned\n");
    }
    if(!fullHeap){
        printf("You aren't returning null when the heap should be full\n");
    }
    if(!uniquePointers){
        printf("Your pointers are not unique for different mallocs\n");
    }

    // a correct implementation should just print out nothing





}