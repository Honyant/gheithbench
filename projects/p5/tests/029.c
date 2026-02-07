#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define HEAP_SIZE (1 << 14)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

int main() {

    // set up the overwriting mallocs test
    char* pointers[100];
    for (int i = 0; i < 100; i++) {
        pointers[i] = (char*)malloc(100);

        for (int j = 0; j < 100; j++) {
            pointers[i][j] = (char)(i * 100 + j);
        }
    }

    // verify that we didn't overwrite the result of any malloc with the other mallocs
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            if (pointers[i][j] != (char) (i * 100 + j)) {
                panic("You likely overwrote memory stored in pointer %d! (0-indexed)\n", i);
            }
        }
    }

    // verify that we didn't overwrite the result of any malloc with the other mallocs
    for (int i = 0; i < 100; i++) {
        free(pointers[i]);
    }

    // check mallocs & frees
    if (mCount != fCount) {
        panic("# of mallocs is not equal to # of frees! - first\n");
    }

    // lets test alignment
    for (int i = 1; i < 100000; i++) {
        // im trying to test a lot of small numbers but some big number allocs will be tested here too
        char* pointer = (char*) malloc(rand() % i + 1);
        if ((long)pointer % 16) {
            panic("Your pointers must always be aligned on a 16-byte boundary!\n");
        }
        free(pointer);
    }

    // check mallocs & frees
    if (mCount != fCount) {
        panic("# of mallocs is not equal to # of frees! - second\n");
    }

    // now we are going to test freeing blocks out of order
    // create some pointers
    char* newPointers[1000];
    int freed[1000];
    for (int i = 0; i < 1000; i++) {
        freed[i] = 0;
        newPointers[i] = (char*) malloc(rand() % 100 + 1);
    }

    // randomly free the pointers
    for (int i = 0; i < 1000; i++) {
        int randBool = rand() % 2;
        if (randBool) {
            free(newPointers[i]);
            freed[i] = 1;
        }
    }

    char* fillingPointers[500];
    // lets create some new pointers (these may fill the holes)
    for (int i = 0; i < 500; i++) {
        fillingPointers[i] = (char*) malloc(rand() % 100 + 1); 
    }

    // free all the pointers that were originally there
    for (int i = 0; i < 1000; i++) {
        if (!freed[i]) {
            free(newPointers[i]);
        }
    }

    // free the rest of the pointers
    for (int i = 0; i < 500; i++) {
        free(fillingPointers[i]);
    }

    // check mallocs & frees
    if (mCount != fCount) {
        panic("# of mallocs is not equal to # of frees! - third\n");
    }

    printf("Success!\n");

    return 0;
}
