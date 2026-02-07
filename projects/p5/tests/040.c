#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define M 1000
#define N 1200000

#define HEAP_SIZE (1 << 20)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

void* last[M] = {};

int main() {

    for (int i=1; i<M; i++) {
        last[i] = malloc(i);
        if (last[i] == 0) {
             panic("*** failed to allocate %d\n",i);
         }
    }
    for (int i=1; i<M; i++)
        if (((uintptr_t)last[i]) % 16)
            panic("Misaligned ptr: i = %d\n", i);
    
    //free in a different order

    for (int i=M-1; i>0; i-=2)
        free(last[i]);
    for (int i=M-2; i>0; i-=2)
        free(last[i]);


    //make sure all recombined

    void* ptr = malloc(HEAP_SIZE - 1000);

    if (ptr == 0)
        panic("*** failed to allocate large chunk after all frees.\n");


    free(ptr);


    /*

    After this is only t0 but with bigger M, N

    */


    for (uint32_t i=0; i<M; i++) {
        last[i] = 0;
    }

    long m1 = mCount;
    long f1 = fCount;

    for (uint32_t i=0; i<N; i++) {
        uint32_t x = rand() % M;
        if (last[x] != 0) {
            free(last[x]);
            last[x] = 0;
        }
        
        size_t sz = (size_t) (rand() % 1000 + 1);
        last[x] = malloc(sz);
        if (last[x] == 0) {
            panic("*** failed to allocate %d\n",sz);
        }
        char* ptr = (char*) last[x];
        ptr[0] = 66;
        ptr[sz-1] = 77;
    }

    for (uint32_t i=0; i<M; i++) {
        if (last[i] != 0) {
            free(last[i]);
        }
    }

    long m2 = mCount - m1;
    long f2 = fCount - f1;
    

  
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
