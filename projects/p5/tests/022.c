#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define HEAP_SIZE 2048

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE / sizeof(long)];

// 0-16 to test small pointers and alignment; 17 to test edge case
void *pointers[18];

int main()
{
    // mallocs pointers of size 0 through 16
    for (int i = 0; i <= 16; i++)
    {
        void *pointer = malloc(i);
        pointers[i] = pointer;
    }

    // checks alignment of each pointer
    bool misaligned = false;
    for (int i = 0; i < 17; i++)
    {
        if (((uintptr_t)pointers[i]) % 16 != 0)
        {
            panic("Pointer of size %d is not misaligned to 16 bytes. Womp womp :(\n", i);
            misaligned = true;
        }
    }
    if (!misaligned)
    {
        printf("Yay! All normal pointers are aligned!\n");
    }

    // free pointers
    for (int i = 0; i < 17; i++)
    {
        free(pointers[i]);
    }

    // edge case: allocating very very very large heap size
    void *large_heap = malloc(HEAP_SIZE - 64);
    if (large_heap != NULL && (((uintptr_t)large_heap) % 16 != 0))
    {
        panic("Large heap pointer is misaligned. \n");
    }
    else
    {
        printf("Yay! You got the big heap too!\n");
    }

    free(large_heap);

    return 0;
}