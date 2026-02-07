#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"
#include <stdbool.h>

#define HEAP_SIZE (1 << 20)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

bool check_alignment(void* ptr) {
    if ((uintptr_t)ptr % 16 != 0) {
        printf("failed alignment check\n");   
        return false;
    }
    return true;
}

int main() {
    // malloc a huge amount of data
    // should be null
    void* big_malloc = malloc(1 << 21);
    if (big_malloc != NULL) {
        printf("Was somehow able to malloc bigger than the heap size/n");
    }

    // malloc a huge amount of data that should still be valid
    // (as long as metadata size is less than 1000 bytes, which it should be)
    void* slightly_less_big_malloc = malloc(HEAP_SIZE - 1000);
    if (slightly_less_big_malloc == NULL) {
        printf("Wasn't able to malloc a valid amount of data\n");
    }

    // free the last malloc
    // the entire heap should now be free
    free(slightly_less_big_malloc);

    void* ptr = malloc(112);

    // for any reasonable metadata size 
    // we should definitely be able to malloc 100 of these blocks
    long threshold = 100; 

    while (ptr != NULL) {
        ptr = malloc(112);
        check_alignment(ptr);
        threshold--;
    }

    if (threshold > 0) {
        printf("Something went wrong\n");
    }

    printf("End of test\n");
    return 0;
}

