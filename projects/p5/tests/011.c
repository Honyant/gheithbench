#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define N 10000

#define HEAP_SIZE (1 << 20)
#define LARGE_BLOCK (HEAP_SIZE - 64) // plenty of leeway for metadata usage

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];
uint64_t chunk_sizes[N];
char* allocated[N];
int ids[N];

int main() {
    // this test tests malloc and free extensively. it also makes sure you are using explicit free list
    // the main idea of this test will allocate a large block of memory at the beginning and end to make sure
    // what is done is the middle is freed up properly
    // in the middle, there will be many small malloc calls and random frees to create interesting merging 
    // situations. furthermore, i write to segments that you gave me so that I can check whether your malloc 
    // assigns any overlapping region or not

    // I allow 32 bytes of meta data here
    char* large_chunk = (char*) malloc(LARGE_BLOCK);
    if(large_chunk == NULL){
        panic("Check that you only require 16 bytes of metadata");
    }

    if ((uintptr_t)large_chunk % 16 != 0) panic("Your large chunk is not aligned correctly!");

    for (int i = 0; i < LARGE_BLOCK; i++) {
        if (((char *)the_heap + heap_size) <= (large_chunk + i)) panic("The end of the block went out of bounds!!");
        large_chunk[i] = (char) 123;
    }

    free(large_chunk);



    // test pointers returned by malloc are valid
    // i write to the malloced pointers to make sure that they did not get tampered with when freeing
    // the id written to each block is the id variable

    char id = 0;
    for (int i = 0; i < N; i++) {
        chunk_sizes[i] = (rand() % 16) + 1;
        allocated[i] = (char *) malloc(chunk_sizes[i]);
        if (allocated[i] == 0) panic("Your malloc didn't allocate anything here!");
        if ((uintptr_t)allocated % 16 != 0) panic("Your malloc-ed chunk is not aligned correctly!");
        ids[i] = (char)id;
        for (int j = 0; j < chunk_sizes[i]; j++) {
            allocated[i][j] = (char)id;
        }

        id++;

        // free some random block 50% of the time
        uint64_t x = rand() % 10;
        if (x >= 4 && x <= 9) {
            x = rand() % (i + 1);
            while (allocated[x] == 0)
                x = rand() % (i + 1);

            char *to_free = allocated[x];

            for (int j = 0; j < chunk_sizes[x]; j++) {
                if (to_free[j] != ids[x]) panic("Your malloc assigned overlapping regions!");
            }

            free(to_free);

            allocated[x] = 0;
        }
        
    }

    for (int i = 0; i < N; i++) {
        if (allocated[i] == 0) continue;

        char *to_free = allocated[i];

        for (int j = 0; j < chunk_sizes[i]; j++) {
            if (to_free[j] != ids[i]) panic("Your malloc assigned overlapping regions!");
        }

        free(to_free);
    }


    // check to make sure that everything was freed correctly by allocating a large block again
    large_chunk = (char*) malloc(LARGE_BLOCK);
    if(large_chunk == NULL){
        panic("Check that you only require 16 bytes of metadata");
    }

    if ((uintptr_t)large_chunk % 16 != 0) panic("Your large chunk is not aligned correctly!");

    for (int i = 0; i < LARGE_BLOCK; i++) {
        if (((char *)the_heap + heap_size) <= (large_chunk + i)) panic("The end of the block went out of bounds!!");
        large_chunk[i] = (char) 123;
    }

    free(large_chunk);

    printf("%s", "You Passed!!!!!\n");
}