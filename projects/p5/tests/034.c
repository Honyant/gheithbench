// This test case covers two basic patterns of allocation and freeing.
// Pass 1 tests stack-like allocation: blocks are allocated in sequence and then freed in reverse order of allocation.
// Pass 2 tests queue-like allocation: blocks are allocated in sequence and then freed in forward order of allocation.
// To keep track of all the allocated pointers, the program stores a linked list, mostly within the heap itself.
// Thus, this test case ensures that allocated blocks can actually be written to without overwriting metadata or other allocated blocks.
// It also uses a majority of the heap, so the second pass ensures that blocks freed on the first pass are actually made available.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define HEAP_SIZE (1 << 24)
#define BLOCK_SIZE (1 << 5)
#define NODE_MAX_SIZE (1 << 6)
#define HEAP_ENDS (1 << 8)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];
long num_allocations = (HEAP_SIZE - HEAP_ENDS) / NODE_MAX_SIZE; // about 256000 allocations per pass

int main() {

	// PASS 1
	// malloc many blocks
	void* last = NULL;
	for (int i = 0; i < num_allocations; i++) {
		void* cur = malloc(BLOCK_SIZE);
		if (!cur) {
			panic("*** failed to allocate on pass 1, call %d\n", i);
		}
		*((void**) cur) = last;
		last = cur;
	}

	// free in reverse order
	while (last) {
		void* cur = last;
		last = *((void**) cur);
		free(cur);
	}

	// PASS 2
	// malloc many blocks
	last = NULL;
	void* first;
	for (int i = 0; i < num_allocations; i++) {
		void* cur = malloc(BLOCK_SIZE);
		if (!cur) {
			panic("*** failed to allocate on pass 2, call %d\n", i);
		}
		if (last) {
			*((void**) last) = cur;
		}
		else {
			first = cur;
		}
		last = cur;
	}
	*((void**) last) = NULL;

	// free in same order
	while (first) {
		void* cur = first;
		first = *((void**) cur);
		free(cur);
	}
	
	printf("Done!\n");
	return 0;
}
