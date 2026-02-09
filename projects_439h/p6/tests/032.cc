#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"


/*
This test does 100 mmaps and then tests 10 unmaps in middle of VMEs.
*/
// Pseudorandom number generator using mod, multiply, and add
uint32_t pseudo_random(uint32_t seed) {
    static const uint32_t multiplier = 1664525;
    static const uint32_t increment = 1013904223;
    return (multiplier * seed + increment);
}

/* Called by one CPU */
void kernelMain(void) {

    using namespace VMM;

    uint32_t seed = 12345; // Initial seed for the pseudorandom number generator
    uint32_t base_address = 0x80000000;

    // Simple loop to mmap random sizes
    for (int i = 0; i < 100; ++i) {
        // Generate a random size between 1 and 10 pages
        uint32_t random_pages = (pseudo_random(seed) % 10 + 1);
        uint32_t random_size = random_pages * 4096;
        seed = pseudo_random(seed); // Update seed for next iteration

        auto p = (char*) naive_mmap(random_size, false, StrongPtr<Node>{}, 0);
        CHECK(p == (char*) base_address);
        Debug::printf("*** Iteration %d: Mapped %u bytes (%u pages) at pointer %x\n", i, random_size, random_pages, p);

        // Increment the base address for the next mmap based on the allocated size
        base_address += random_size;
    }

    // Unmapping section
    Debug::printf("\n*** Starting Random Unmaps ***\n");
    
    // Perform 10 random unmaps
    for (int i = 0; i < 10; ++i) {
        // Generate a random address for unmap within the range of [0x80000000, 0x8024a000]
        uint32_t random_offset = (pseudo_random(seed) % (0x8024a000 - 0x80000000));
        seed = pseudo_random(seed); // Update seed for next iteration

        void* unmap_address = (void*)(0x80000000 + random_offset);
        naive_munmap(unmap_address);
        Debug::printf("*** Unmap %d: Unmapped address %x\n", i, unmap_address);
    }
}
