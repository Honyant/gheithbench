#include "heap.h"
#include "panic.h"
#include <stdlib.h>

long mCount = 0;
long fCount = 0;

void* malloc(size_t bytes) {
    mCount += 1;
    return 0;
}

void free(void* p) {
    fCount += 1;
}
