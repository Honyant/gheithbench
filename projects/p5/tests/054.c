#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define HEAP_SIZE (1 << 15)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];


//checks if pointer is not null, check if pointer is aligned correctly, and writes to beginning and end of memory
bool is_pointer_valid(void* pointer, size_t size) {
    if (pointer == 0) {
        return false;
    }
    if ((uint64_t) pointer % 16 != 0) {
        return false;
    }

    char* ptr = (char*) pointer;
    ptr[0] = 66;
    ptr[size-1] = 77;
    return true;
}


#define N 6
void* pointers[N] = {};

int main(){
    for(int i = 0; i < N; i++){
        pointers[i] = 0;
    }

    long m1 = mCount;
    long f1 = fCount;

    for(int i = 0; i < N; i++){
        size_t size = 128;
        pointers[i] = malloc(size);
        bool valid = is_pointer_valid(pointers[i], size);
        if (!valid) {
            panic("The returned pointer was not valid on malloc number %d.\n", i);
        }
    }

    //test freeing surrounded by 2 allocated blocks
    free(pointers[2]);
    //test freeing with free block on left side
    free(pointers[3]);
    //test freeing with a free block on right side
    free(pointers[1]);


    //test freeing with a free block on both sides
    free(pointers[5]);
    free(pointers[4]);

    free(pointers[0]);

    //test mallocing something almost the size of the whole heap, to make sure free blocks were connected
    void* p1 = malloc(heap_size - 48);
    bool valid = is_pointer_valid(p1, heap_size - 48);
    if (!valid) {
        panic("The returned pointer was not valid on the big malloc.\n");
    }

    free(p1);
    

    //test if malloc too big returns null
    void* p2 = malloc(heap_size + 16);
    if(p2 != 0) {
        panic("You returned a non-null pointer for a malloc that was too big.\n");
    }

    free(p2);

    long m2 = mCount - m1;
    long f2 = fCount - f1;

    if(m2 == 8 && f2 == 8){
        printf("good job\n");
    }

    


}