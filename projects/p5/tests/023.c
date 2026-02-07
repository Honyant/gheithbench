#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include "heap/heap.h"
#include "heap/panic.h"


#define HEAP_SIZE (1 << 20)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

int main() {
    long* last[100000];
    // basic malloc and free
    void* a = malloc(10);
    if (a == 0) {
        panic("*** failed to allocate pointer b\n");
    }
    if ((uintptr_t)a % 16 != 0) {
        panic("a is misaligned\n");
    }
    free(a);
    void* veryLarge = malloc(HEAP_SIZE - 64); // should be able to malloc something very large after freeing 
    free(veryLarge);

    // checks to make sure that the same pointer is not returned every time
    void* b = malloc(31);
    if (b == 0) {
        panic("*** failed to allocate pointer b\n");
    }
    if ((uintptr_t)b % 16 != 0) {
        panic("b is misaligned\n");
    }

    void* c = malloc(31);
    if (c == 0) {
        panic("*** failed to allocate pointer c\n");
    }
    if ((uintptr_t)c % 16 != 0) {
        panic("c is misaligned\n");
    }

    void* d = malloc(31);
    if (d == 0) {
        panic("*** failed to allocate pointer d\n");
    }
    if ((uintptr_t)d % 16 != 0) {
        panic("d is misaligned\n");
    }

    if ((uintptr_t)b == (uintptr_t)c || (uintptr_t)c == (uintptr_t)d || (uintptr_t)b == (uintptr_t)d) {
        panic("*** different malloced pointers share the same value\n");
    }
    free(b);
    free(c);
    free(d);
    void* veryLarge2 = malloc(HEAP_SIZE - 64); // should be able to malloc something very large after freeing 
    free(veryLarge2);

    // general mallocing test
    for (int i = 0; i < 1000; i++) {
        last[i] = malloc(8);
        if (last[i] == 0) {
            panic("*** failed to allocate at i = %d\n",i);
        }
        if ((uintptr_t)last[i] % 16 != 0) {
            panic("pointer at last %d is misaligned\n", i);
        }
        long* ptr = (long*) last[i];
        ptr[0] = i; 
    }

    for (int i = 0; i < 1000; i++) {
        if (*(last[i]) != i) {
            panic("The array at %d does not contain the value %d\n", i, i);
        }
    }

    for (int i = 0; i < 1000; i++) {
        free(last[i]);
    }

    printf("Test case over!\n");
    return 0;
}
