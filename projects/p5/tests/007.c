#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define M 15000 //How many maximum 64 byte allocs will be made
#define O 30 //How many times the loop will be run
#define HEAP_SIZE (1 << 20)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE / sizeof(long)];

void *last[M] = {};

//To pass this test your heap will need to be both memory and time efficient
int main()
{

    for (uint32_t i = 0; i < M; i++)
    {
        last[i] = 0;
    }

    long m1 = mCount;
    long f1 = fCount;
    for (uint32_t j = 0; j < O; j++)
    {
        for (uint32_t i = 0; i < M; i++)
        {
            size_t sz = (size_t)(rand() % 64 + 1);
            last[i] = malloc(sz);
            if (last[i] == 0)
            {
                panic("*** failed to allocate %d\n", sz);
            }
            char *ptr = (char *)last[i];
            ptr[0] = 66;
            ptr[sz - 1] = 77;
        }
        for (uint32_t i = 0; i < M; i++)
        {
            free(last[i]);
        }
    }

    long m2 = mCount - m1;
    long f2 = fCount - f1;

    if (m2 != f2)
    {
        printf("m2 %ld\n", m2);
        printf("f2 %ld\n", f2);
    }
    else
    {
        printf("count match\n");
    }

    if (m2 != M * O)
    {
        printf("*** wrong count %ld\n", m2);
    }
    else
    {
        printf("count ok\n");
    }

    return 0;
}
