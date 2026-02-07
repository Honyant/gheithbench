#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define HEAP_SIZE (1024)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

int main() {
    // Check a few corner cases

    long m1 = mCount;
    long f1 = fCount;

    void* zero = malloc(0);
    free(zero);

    void* large = malloc(1025);
    if (large != NULL) {
        printf("large malloc not null\n");
    }

    free(large);
    free(0);
    free(NULL);

    printf(":)\n");

    long m2 = mCount;
    long f2 = fCount;

    printf("%ld %ld\n", m2 - m1, f2 - f1);
    return 0;
}
