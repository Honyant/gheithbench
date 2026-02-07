#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define HEAP_SIZE (1 << 20)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

void checkAlignment(void* ptr){
    uint32_t* u32 = (uint32_t *) ptr;
    //uint32_t* heapstart = (uint32_t *) &the_heap;
    if((uint64_t) u32 % 4 != 0 || ptr == 0){
        panic("Not aligned\n");
    }
}
int main() { 
    
    // Test 0: 
    // Allignment check 
    uint32_t* t = malloc(10);
    checkAlignment(t); 
    free((void *) t);

    // Test 1: 
    // Just to make sure it's possible to malloc 0...
    uint32_t* zp = malloc(0); 
    if(zp){
        // now the compiler thinks zp has a use muaha
    }
    free((void *) zp);

    // Test 2: 
    // Illegal malloc
    if(malloc(1 << 21) != 0){
        panic("Not returning 0 for full malloc!\n");
    }

    
    // Test 3: 
    // Pointer check
    uint32_t* start = malloc(100); 
    checkAlignment(start);
    *start = 900;
    if(*start != 900){
        panic("Test 3 failed.\n");
    }
    for(int i = 0; i < 25; i++){
        *(start + i) = 8989;
    }
    if(*(start + 4) != 8989){
        panic("Test 3 failed.\n");
    }
    free((void *) start); 
   
    
    
    // Test 4: 
    // Empty chunk recombination 
    uint32_t* p1 = malloc(heap_size / 3);
    uint32_t* p2 = malloc(heap_size / 3);
    checkAlignment(p1);
    checkAlignment(p2);
    free((void *) p1);
    free((void *) p2); 
    uint32_t* p3 = malloc(heap_size / 2); 
    checkAlignment(p3);
    if(p3 == 0){
        panic("Test 4 failed\n");
    }
    free((void *) p3);

    // Test 5: 
    // mallocing to malloced data (array of pointers)
    uint64_t** ptrArray = malloc(1000 * sizeof(uint64_t));
    checkAlignment(ptrArray);
    for(int i = 0; i < 1000; i++){
        *(ptrArray + i) = malloc(sizeof(uint64_t));
        checkAlignment(*(ptrArray + i));
        **(ptrArray + i) = (uint64_t) i * i;
    }
    for(int i = 0; i < 1000; i++){
        if(**(ptrArray + i) != (uint64_t) i * i){
            panic("ptrArray not equal in test 5\n"); 
        }
        free((void*) *(ptrArray + i));
    }
    free((void*) ptrArray);

    // Test 6:
    // Max metadata should be 32 bytes
    uint32_t* maxChunk = malloc(heap_size - 32);
    if(maxChunk == 0){
        panic("Too much metadata.\n");
    }
    free((void *) maxChunk);
    
    printf("Tests passed :)\n");
}