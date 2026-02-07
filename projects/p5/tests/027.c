#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define HEAP_SIZE (1 << 30)
long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE / sizeof(long)];

int n = 5000;
long allOnes = 0xffffffffffffffff;

//check validity of returned pointer
void checkPointer(void *p, size_t bytes)
{
    if (p == 0) return;

    if (p < (void *)the_heap)
    {
        panic("pointer is before the_heap when mallocing %lu bytes, malloc #%lu\n", bytes, mCount);
    }

    if (p > (void *)the_heap + heap_size)
    {
        panic("pointer is after the_heap when mallocing %lu bytes, malloc #%lu\n", bytes, mCount);
    }

    if (p > (void *)the_heap + heap_size - bytes)
    {
        panic("pointer is too close to the end of the_heap when mallocing %lu bytes, malloc #%lu\n", bytes, mCount);
    }

    if ((long)p % 16 != 0)
    {
        panic("pointer is not aligned to 16 when mallocing %lu bytes, malloc %lu\n", bytes, mCount);
    }
}

//the test
void runTest(int run) {

    //creates array of n pointers
    //each pointer will be the start of an array of longs
    long *pointers[n];
    //array to keep track of the sizes of the long arrays
    int sizes[n];

    for (int i = 0; i < n; i++)
    {
        //%2000 so that it definitely won't get too large
        uint32_t x = 0;
        while (x == 0) x = (rand() % 20000);

        //number of bytes for the whole array of longs
        size_t bytes = sizeof(long) * x;
        pointers[i] = (long *)malloc(bytes);
        sizes[i] = x;
        long *arr = pointers[i];

        //set all data in the array to 1
        //tests that it doesn't mess with data
        for (int j = 0; j < x; j++)
        {
            *(arr + j) = allOnes;
        }

        checkPointer(pointers[i], bytes);
    }

    for (int i = 0; i < n; i++)
    {
        long *arr = pointers[i];

        //check that all elements of the array are still what I set it to
        for (int j = 0; j < sizes[i]; j++)
        {
            if (*(arr + j) != allOnes)
            {
                panic("memory from mallocing %lu has been changed\n", sizes[i] * sizeof(long));
            }
        }

        //free all the pointers
        if (pointers[i] != 0) free(pointers[i]);
    }

    //wanted to check mCount too but printf messes with that
    //so check fCount to at least make sure the number of frees is right
    if (fCount == (n*(run+1)))
    {
        printf("successfully freed! (probably) round %i\n", run);
    } else {
        panic("free count doesn't match after round %i\n", run);
    }
}

int main()
{
    //10 rounds of mallocing and freeing
    //ideally will make sure that freed space can be reused
    for (int i = 0; i < 10; i++)
    {
        runTest(i);
    }

    //finished
    printf("yay done :D\n");
}