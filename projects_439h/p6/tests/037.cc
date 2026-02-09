#include "kernel.h"
#include "debug.h"
#include "ext2.h"
#include "vmm.h"

/* This test case tests three things: 
 * Very large (sparse) mmaps/munmaps 
 * Freeing from the middle of an mmapped range
 * Behavior when out of virtual memory, including the arithmetic for checking
 */ 

void kernelMain(void) {
	using namespace VMM;

	void *p = naive_mmap(0x60000000, false, {}, 0);
	ASSERT(p == (void*)0x80000000);

	void *q = naive_mmap(0x18000000, false, {}, 0);
	ASSERT(q == nullptr); // out of virtual memory in private space
	q = naive_mmap(0x28000000, false, {}, 0);
	ASSERT(q == nullptr); // integer overflow is possible
	q = naive_mmap(0x8000000, false, {}, 0);
	ASSERT(q == (void*)0xE0000000); // should work and immediately follow previous pointer
	naive_munmap(q);

	naive_munmap((void*) ((char*) p + 0x12910827)); // unmap from random pointer within range
	
	q = naive_mmap(0x18000000, false, {}, 0);
        ASSERT(q == (void*)0x80000000);
	naive_munmap(q);


	void *r = naive_mmap(0x08000000, true, {}, 0);
	ASSERT(r == (void*)0xF0000000);
	
	void *s = naive_mmap(0x09000000, true, {}, 0);
	ASSERT(s == nullptr); // out of virtual memory in shared space
	s = naive_mmap(0x05000000, true, {}, 0);
	ASSERT(s == (void*)0xF8000000);
	naive_munmap(s);

	naive_munmap((void*) ((char*) r + 0x4726382));

	s = naive_mmap(0x09000000, true, {}, 0);
	ASSERT(s == nullptr); // shared mappings should not be unmapped


	Debug::printf("*** All done!\n");
}
