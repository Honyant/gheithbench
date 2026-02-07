#include "heap.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "panic.h"
#define retPtr(x) (void*)(the_heap + ((long)x))
#define updateFooter(x) if(x - the_heap[x + 1] + 2 < heap_8bytes) the_heap[x - the_heap[x + 1] + 2] = abs(the_heap[x + 1])

long mCount = 0;
long fCount = 0;
bool uninitialized = true;
long free_start = 0;
int heap_8bytes = 0;
void* malloc(size_t bytes) {
    mCount++;
    if (bytes == 0) {return NULL;}
    long bbytes = ((bytes + 15) >> 4) << 1;  // byte align to 16
    if (uninitialized) {
        heap_8bytes = ((heap_size + 15) >> 4) << 1;  // byte align to 16
        the_heap[1] = -(heap_8bytes - 2);
        uninitialized = false;
    }
    int curr = free_start;
    while (curr < heap_8bytes) {
        int size = the_heap[curr + 1];
        if (size > 0) {
            curr += size + 2;
            continue;
        }
        if (size == 0) {
            break;
        }
        size = -size;
        if (bbytes > size){ // if this is true then skip
            curr += size + 2;
            continue;
        }
        if (size - bbytes > 4) {// if this is true then split
            the_heap[curr + 1] = bbytes;  // set header size of allocated block
            the_heap[curr + bbytes + 2] = bbytes;  // set footer size of allocated block
            the_heap[curr + bbytes + 2 + 1] = -(size - bbytes - 2);  // set header size of free block
            updateFooter(curr + bbytes + 2);
            free_start = curr;
            return retPtr(curr + 2);
        } else{
            the_heap[curr + 1] = size; // mark block as allocated
            free_start = curr;
            return retPtr(curr + 2);
        }
    }
    return NULL;
}
void free(void* ptr) {
    fCount++;
    
    if (ptr == NULL || uninitialized || (long*)ptr < the_heap || (long*)ptr > the_heap + heap_8bytes) return;
    int curr = ((long*)ptr - the_heap) - 2;
    the_heap[curr + 1] = -abs(the_heap[curr + 1]); // mark block as free
    int next = curr - the_heap[curr + 1] + 2;
    if (curr < free_start) free_start = curr;

    if (next < heap_8bytes && the_heap[next + 1] < 0) {  // check if next block is within bounds, and merge if it is
        the_heap[curr + 1] += the_heap[next + 1] - 2;  // merge
        updateFooter(curr);
    }

    int prev = curr - the_heap[curr] - 2;
    if (prev >= 0 && the_heap[prev + 1] < 0) {  // check if previous block is within bounds, and merge if it is
        the_heap[prev + 1] += the_heap[curr + 1] - 2;  // merge
        updateFooter(prev);
        if (prev < free_start) free_start = prev;
    }
}