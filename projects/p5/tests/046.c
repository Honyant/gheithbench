#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define MB 10
#define HEAP_SIZE (1 << 18) // heap size (bytes)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE / sizeof(long)];

void *last[MB] = {};

int main()
{
    for (uint32_t i = 0; i < MB; i++)
    {
        last[i] = NULL;
    }

    // allocate large number of small blocks
    for (uint32_t i = 0; i < 50000; i++)
    {
        uint32_t x = rand() % MB;
        if (last[x] != NULL)
        {
            free(last[x]);
            last[x] = NULL;
        }
        size_t sz = (size_t)(rand() % 64 + 1);
        last[x] = malloc(sz);
        if (last[x] == NULL)
        {
            panic("*** failed to allocate %d\n", sz);
        }
    }

    for (uint32_t i = 0; i < MB; i++)
    {
        if (last[i] != NULL)
        {
            free(last[i]);
            last[i] = NULL; // Set to NULL after freeing
        }
    }

    // allocate large blocks
    for (int i = 0; i < 500; i++)
    {
        for (int j = 0; j < MB; j++)
        {
            if (last[j] != NULL)
            {
                free(last[j]);
                last[j] = NULL; // Set to NULL after freeing
            }
        }

        for (int j = 0; j < MB; j++)
        {
            size_t sz = (size_t)(rand() % 4096 + 1);
            last[j] = malloc(sz);
            if (last[j] == NULL)
            {
                panic("*** failed to allocate %d\n", sz);
            }
        }
    }

    // allocate zero bytes
    last[1] = malloc(0);

    for (uint32_t i = 0; i < MB; i++)
    {
        if (last[i] != NULL)
        {
            free(last[i]);
            last[i] = NULL; // Set to NULL after freeing
        }
    }

    printf("all done!! \n");

    return 0;
}

