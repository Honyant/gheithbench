#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"
#include <stdbool.h>


#define HEAP_SIZE 128

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

bool validPointer (void* ptr) {
    if((long) ptr % 16 != 0){
        return false;
    }

    if ((long*) ptr - the_heap < 0 || (long*) ptr - the_heap > 16) {
        return false;
    }

    return true;
};

int main() {

    void* ptrs[HEAP_SIZE/sizeof(long)];
    long num_of_ptrs = 0;

    // checking oversized malloc
    void* ptr = malloc(129);
    bool oversized_check = true;
    if(ptr != NULL){
        oversized_check = false;
    }

    
    while (ptr != NULL) {
        ptr = malloc(2);
        ptrs[num_of_ptrs] = ptr;
        num_of_ptrs++;
    }

    bool validPointers_check = true;

    for(int i = 0; i < num_of_ptrs; i++){
        if(! validPointers_check && !validPointer(ptrs[i])) {
            validPointers_check = false;
        }
        free(ptrs[i]);
    }

    bool fragmentation_check = true;
    // checking that there's no fragmentation after freeing all
    for(int i = 0; i < num_of_ptrs; i++){  
        ptr = malloc(2);
        if(ptr == NULL){
            fragmentation_check = false;
        }
        ptrs[i] = ptr;
    }  

    for(int i = 0; i < num_of_ptrs; i++){
        free(ptrs[i]);
    }

    // fragmentation check 2
    bool fragmentation_check2 = true;
    void* largeptr = malloc(64);
    num_of_ptrs = 0;

    while (ptr != NULL) {
        ptr = malloc(2);
        ptrs[num_of_ptrs] = ptr;
        num_of_ptrs++;
    }

    free(largeptr);
    
    for(int i = 0; i < num_of_ptrs; i++){
        ptr = malloc(2);
        if(ptr == NULL) {
            fragmentation_check2 = false;
        }
        ptrs[num_of_ptrs + i] = ptr;
    }

    for(int i = 0; i < num_of_ptrs; i++){
        free(ptrs[i]);
        free(ptrs[num_of_ptrs + i]);
    }

    if (oversized_check) {
        printf("Passing oversized malloc test\n");
    } else {
        printf("Failing oversized malloc test\n");
    }

    if (validPointers_check) {
        printf("Passing valid pointers test\n");
    } else {
        printf("Failing valid pointers test\n");
    }

    if (fragmentation_check) {
        printf("Passing fragmentation test\n");
    } else {
        printf("Failing fragmentation test\n");
    }

    if (fragmentation_check2) {
        printf("Passing fragmentation2 test\n");
    } else {
        printf("Failing fragmentation2 test\n");
    }

    return 0;
}
