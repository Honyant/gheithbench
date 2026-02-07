#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define HEAP_SIZE (1 << 12) // 4096

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

int main() {
    
    void* heapPointers[42];
    long myfCounter = 0;
    long mymCounter = 0;
    // Initializes the heap
    for (int i=0; i<42; i++) {
        mymCounter += 1;
        heapPointers[i] = malloc(60);
        // Checks alignment
        if ((size_t) (heapPointers[i]) % 16 != 0) {
            panic("BLOCK IS UNALIGNED AT THE %ldTH MALLOC\n", i);
        }
        if (heapPointers[i] == NULL) {
            panic("WHEN MALLOC RETURNING NULL AT %ldTH MALLOC\n", i);
        }
    }

    // Free with no merge
    free(heapPointers[10]);
    myfCounter += 1;

    // Free with left merge
    free(heapPointers[19]);
    free(heapPointers[20]);
    myfCounter += 2;

    // Free with right merge
    free(heapPointers[40]);
    free(heapPointers[39]);
    myfCounter += 2;

    // Free with both left and right merge
    free(heapPointers[5]);
    free(heapPointers[7]);
    free(heapPointers[6]);
    myfCounter += 3;

    // Free remaining pointers
    for (int i=0; i<42; i++) {
        if (i!=10 && i!=19 && i!=20 && i!=40 && i!=39 && i!=5 && i!=7 && i!=6) {
            free(heapPointers[i]);
            myfCounter += 1;
        }
    }

    if (malloc(5000) != NULL) {
        panic("ATTEMPTING TO MALLOC SPACE THAT ISN'T AVAILABLE\n");
    }
    mymCounter += 1;
    if (myfCounter == fCount && mymCounter == mCount) {
        printf("Yay successfully passing this testcase!\n");
    }
    else {
        if (myfCounter != fCount && mymCounter != mCount) {
            panic("fCount AND mCount ARE BOTH INCORRECT\n");
        }
        else if (myfCounter != fCount) {
            panic("fCount IS INCORRECT\n");
        }
        else {
            panic("mCount IS INCORRECT\n");
        }
    }
    return 0;
}
