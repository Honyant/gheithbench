#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"
#include "physmem.h"
#include "vmm.h"
#include "barrier.h"
#include "atomic.h"

// Define the boundaries of the user memory space
#define START_ADDRESS 0x80000000
#define PAGE_SIZE_     0x1000
#define TOTAL_PAGES   0x7000

void kernelMain(void){
    using namespace VMM;
    
    char** mapped_pages = new char*[TOTAL_PAGES];

    Debug::printf("*** Mapping Entire Address Space\n");
    for (uint32_t i = 0; i < TOTAL_PAGES; i++) {
        mapped_pages[i] = (char*) naive_mmap(PAGE_SIZE_, false, StrongPtr<Node>{}, 0);
        ASSERT(mapped_pages[i] != nullptr);
        ASSERT((uint32_t) mapped_pages[i] == START_ADDRESS + (i * PAGE_SIZE_));
    }

    Debug::printf("*** Fragmenting the Address Space\n");
    for (uint32_t i = 0; i < TOTAL_PAGES; i += 4) {
        if (mapped_pages[i] != nullptr) {
            naive_munmap((void*) mapped_pages[i]);
            mapped_pages[i] = nullptr;
        }
    }

    Debug::printf("*** Reallocating Fragmented Pages\n");
    for (uint32_t i = 0; i < TOTAL_PAGES; i += 4) {
        mapped_pages[i] = (char*) naive_mmap(PAGE_SIZE_, false, StrongPtr<Node>{}, 0);
        ASSERT(mapped_pages[i] != nullptr);
        ASSERT((uint32_t) mapped_pages[i] == START_ADDRESS + (i * PAGE_SIZE_));
    }

    Debug::printf("*** Continuous Fragmentation and Reallocation Loop\n");
    for (uint32_t loop = 0; loop < 500; loop++) {
        for (uint32_t i = 0; i < 500; i++) {
            if (mapped_pages[i] != nullptr && i == (loop % 500)) {
                naive_munmap((void*) mapped_pages[i]);
                mapped_pages[i] = nullptr;
            }
        }

        for (uint32_t i = 0; i < 500; i++) {
            if (mapped_pages[i] == nullptr) {
                mapped_pages[i] = (char*) naive_mmap(PAGE_SIZE_, false, StrongPtr<Node>{}, 0);
                ASSERT(mapped_pages[i] != nullptr);
            }
        }
    }

    Debug::printf("*** Unmapping All Pages\n");
    for (uint32_t i = 0; i < TOTAL_PAGES; i++) {
        if (mapped_pages[i] != nullptr) {
            naive_munmap((void*) mapped_pages[i]);
            mapped_pages[i] = nullptr;
        }
    }
}
