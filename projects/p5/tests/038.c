#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define HEAP_SIZE (1 << 20)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE / sizeof(long)];

void *pointers[1000000] = {};

bool checkAlignment(void *pointer)
{
    return (uintptr_t)pointer % 16 == 0;
}
int main()
{
    // long m1 = mCount;
    // long f1 = fCount;

    int numMalloc = 100;
    // test many malloc + free + alignment
    for (int i = 0; i < numMalloc; i++)
    {
        pointers[i] = malloc(rand() % 500 + 16);
        if(pointers[i] == 0)
        {
            panic("*** failed to allocate %d\n", 16);
        }
        if(!checkAlignment(pointers[i]))
        {
            panic("Error: misaligned pointer %ld\n", (uint64_t)pointers[i] % 16);
        }
    }
    for(int i = 0; i < numMalloc; i++)
    {
        free(pointers[i]);
    }
    printf("Passed test 1\n");


    numMalloc = heap_size / 256;
    // test 2: test fragmentation + alignment
    for (int i = 0; i < numMalloc; i++)
    {
        pointers[i] = malloc(16);
        if (pointers[i] == 0)
        {
            panic("*** failed to allocate %d\n", 16);
        }
        if (!checkAlignment(pointers[i]))
        {
            panic("Error: misaligned pointer %ld\n", (uint64_t)pointers[i] % 16);
        }
        if (i % 2 == 0)
        {
            free(pointers[i]);
        }
    }
    void *testp = malloc(16);
    if (testp != 0)
    {
        printf("Passed test 2\n");
    }
    else
    {
        printf("Failed test 2\n");
    }
    free(testp);

    for (int i = 0; i < numMalloc; i++)
    {
        if (i % 2 != 0)
        {
            free(pointers[i]);
        }
    }
    // test 2.1 test mallocing to list again
    for (int i = 0; i < numMalloc; i++)
    {
        pointers[i] = malloc(16);
        if (pointers[i] == 0)
        {
            panic("*** failed to allocate %d\n", 16);
        }
        if (!checkAlignment(pointers[i]))
        {
            panic("Error: misaligned pointer %ld\n", (uint64_t)pointers[i] % 16);
        }
    }
    printf("Passed test 3\n");

    // test 2.2 shuffle free (shuffles pointers using knuth shuffle) (should test all edge cases of free)
    for (int i = numMalloc - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        void *temp = pointers[i];
        pointers[i] = pointers[j];
        pointers[j] = temp;
    }
    for (int i = 0; i < numMalloc; i++)
    {
        free(pointers[i]);
    }

    // third pass of mallocing to list
    for (int i = 0; i < numMalloc; i++)
    {
        pointers[i] = malloc(16);
        if (pointers[i] == 0)
        {
            panic("*** failed to allocate %d\n", 16);
        }
        if (!checkAlignment(pointers[i]))
        {
            panic("Error: misaligned pointer %ld\n", (uint64_t)pointers[i] % 16);
        }
    }
    // free all
    for (int i = 0; i < numMalloc; i++)
    {
        free(pointers[i]);
    }
    printf("Passed test 4\n");

    return 0;
}
