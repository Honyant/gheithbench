#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define M 500
#define N 100000

#define HEAP_SIZE (1 << 20)

/* testing heap implementations that bucket into powers of 2:

assuming a 3-byte header + 16-byte alignment
malloc for 2^x+1
    17, 33, 65, 129, 257, 513, 1025, 2049
*/

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

void* last[M] = {};

uint16_t my_sizes[8] = {17, 33, 65, 129, 257, 513, 1025, 2049};

int main() {

    for (uint32_t i=0; i<M; i++) {
        last[i] = 0;
    }

    long m1 = mCount;
    long f1 = fCount;

    // N iterations of malloc/free to random sizes of memory in heap
    for (uint32_t i=0; i<N; i++) {
        uint32_t x = rand() % M; // randomly select to free -> malloc
        if (last[x] != 0) {
            free(last[x]);
            last[x] = 0;
        }

        size_t sz = my_sizes[(rand() % 8)]; // randomly malloc one of the above sizes of bytes

        last[x] = malloc(sz); // returns pointer to heap
        if (last[x] == 0) { // did not malloc :(
            panic("*** failed to allocate %d\n",sz);
        }
        char* ptr = (char*) last[x]; // char pointer to memory malloced
        ptr[0] = 66; // set first byte
        ptr[sz-1] = 77; // set last byte
    }

    // if had alloced, free!!
    for (uint32_t i=0; i<M; i++) {
        if (last[i] != 0) {
            free(last[i]);
        }
    }

    long m2 = mCount - m1;
    long f2 = fCount - f1;
    
    // check for matching malloc/free counts
    if (m2 != f2) {
        printf("m2 %ld\n",m2);
        printf("f2 %ld\n",f2);
    } else {
        printf("count match\n");
    }

    if (m2 != N) {
        printf("*** wrong count %ld\n",m2);
    } else {
        printf("count ok\n");
    }

    return 0;
}
