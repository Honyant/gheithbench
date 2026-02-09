#include "ide.h"
#include "ext2.h"
#include "kernel.h"
#include "physmem.h"
#include "vmm.h"

// utility functions
static inline uint32_t max(uint32_t one, uint32_t two) {
    return one > two ? one : two;
}
static inline uint32_t min(uint32_t one, uint32_t two) {
    return one < two ? one : two;
}
static inline uint32_t abs(uint32_t num) {
    return num > 0 ? num : -num;
}
static inline uint32_t hash(char* ptr) {
    return ((0x12345678 ^ ~((uint32_t) ptr)) % 8191) % 4096;
}

void kernelMain(void) {
    using namespace VMM;

    // can you turn on virtual memory?
    Debug::printf("*** Virtual memory should be on now.\n"); 

    auto ide = StrongPtr<Ide>::make(1, 0); // not testing disk latency    
    auto fs = StrongPtr<Ext2>::make(ide); // extremely simple directory structure
    auto root = fs->root; // only contains one file: "message.txt"
    auto file = fs->find(root, "message.txt"); // will be mmapped and printed later

    // can you allocate ALL of shared memory?
    uint32_t low_size = min(kConfig.localAPIC, kConfig.ioAPIC) - 0xF0000000;
    uint32_t mid_size = abs(kConfig.localAPIC - kConfig.ioAPIC) - PhysMem::FRAME_SIZE;
    uint32_t high_size = (0 - max(kConfig.localAPIC, kConfig.ioAPIC)) - 2 * PhysMem::FRAME_SIZE;
    ASSERT(low_size + mid_size + high_size == (0x0 - 0xF0000000) - (3 * PhysMem::FRAME_SIZE));

    // can you map your virtual address space?
    char* low = (char*) naive_mmap(low_size, true, StrongPtr<Node>(), 0);
    Debug::printf("*** Lower region of shared memory: %x.\n", low);
    char* mid = (char*) naive_mmap(mid_size, true, StrongPtr<Node>(), 0);
    Debug::printf("*** Middle region of shared memory: %x.\n", mid);
    char* high = (char*) naive_mmap(high_size, true, StrongPtr<Node>(), 0);
    Debug::printf("*** High region of shared memory: %x.\n", high);
    // offset should avoid the question mark at the start of the file
    char* top = (char*) naive_mmap(file->size_in_bytes(), true, file, 1); 
    Debug::printf("*** Last page of shared memory: %x.\n", top);

    // do you handle the case where no more space is available?
    ASSERT(naive_mmap(1, true, StrongPtr<Node>(), 0) == nullptr); 

    // can you repeat this process?
    for (uint32_t q = 0; q < 4; q++) {
        Debug::printf("*** \n*** Iteration #%u.\n*** \n", q);

        // can you access (write to) this memory?
        for (char* ptr = low; ptr < low + low_size; ptr += 4 * PhysMem::FRAME_SIZE) {
            ptr[hash(ptr)] = 'l'; // write somewhere inside every fourth page
        }
        Debug::printf("*** Wrote to lower region of shared memory.\n");
        for (char* ptr = mid; ptr < mid + mid_size; ptr += 4 * PhysMem::FRAME_SIZE) {
            ptr[hash(ptr)] = 'm'; // write somewhere inside every fourth page
        }
        Debug::printf("*** Wrote to middle region of shared memory.\n");
        for (char* ptr = high; ptr > (char*) 0xF0000000; ptr += 4 * PhysMem::FRAME_SIZE) {
            ptr[hash(ptr)] = 'h'; // write somewhere inside every fourth page
        }
        Debug::printf("*** Wrote to upper region of shared memory.\n");
        {
            // overwrite exclamation point at end of file with newline
            top[file->size_in_bytes() - 3] = '\n'; 
            Debug::printf("*** Wrote to the very last page of shared memory.\n");
        }

        // is your state consistent (can you read what you wrote)?
        for (char* ptr = low; ptr < low + low_size; ptr += 4 * PhysMem::FRAME_SIZE) {
            ASSERT(ptr[hash(ptr)] == 'l'); // read from the pages you wrote to
        }
        Debug::printf("*** Read from lower region of shared memory.\n");
        for (char* ptr = mid; ptr < mid + mid_size; ptr += 4 * PhysMem::FRAME_SIZE) {
            ASSERT(ptr[hash(ptr)] == 'm'); // read from the pages you wrote to
        }
        Debug::printf("*** Read from middle region of shared memory.\n");
        for (char* ptr = high; ptr > (char*) 0xF0000000; ptr += 4 * PhysMem::FRAME_SIZE) {
            ASSERT(ptr[hash(ptr)] == 'h'); // read from the pages you wrote to
        }
        Debug::printf("*** Read from upper region of shared memory.\n");
        {
            // should print "*** Read from the very last page of shared memory."
            char* ptr = (char*) -PhysMem::FRAME_SIZE; // pointer to the start of the last page of shared memory
            Debug::printf("%s", ptr); // should already be null-terminated
        }

        // do you preserve these allocations?
        naive_munmap(low);
        Debug::printf("*** \"Unmapped\" low region of shared memory.\n");
        naive_munmap(mid);
        Debug::printf("*** \"Unmapped\" middle region of shared memory.\n");
        naive_munmap(high);
        Debug::printf("*** \"Unmapped\" high region of shared memory.\n");
        naive_munmap(high);
        Debug::printf("*** \"Unmapped\" last page of shared memory.\n");
    }

    
}

