#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define M 20
#define N 100000

#define HEAP_SIZE 8 * ((1 << 20) + 200)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

// void* last[M] = {};
void* ptrs[20];
int free_list[5] = {1, 2, 3, 6, 7};
int sz[20];

int main() {

    for (uint32_t i=0; i<M; i++) {
        ptrs[i] = malloc(8 * (1 << i));
        sz[i] = 8 * (1 << i);
    }
    for (uint32_t i=0; i < 5; i++) {
        free(ptrs[i]);
        sz[i] = 0;
    }
    ptrs[6] = malloc(8 * ((1 << 6) + (1 << 5)));
    sz[6] = 8 * ((1 << 6) + (1 << 5));
    for (uint32_t i = 0; i < M; i++) {
        if (sz[i] == 0) continue;
        for (int j = 0; j < M; j++) {
            if (i == j) continue;
            if (ptrs[i] == ptrs[j]) assert(0);
            if (ptrs[j] < ptrs[i] && ptrs[j] + sz[j] - 1 >= ptrs[i]) assert(0);
            if (ptrs[j] > ptrs[i] && ptrs[j] <= (ptrs[i] + sz[i] - 1)) assert(0);
        }
    }
    printf("Successful\n");


    return 0;
}
