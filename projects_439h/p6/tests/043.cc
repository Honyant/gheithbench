#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"


/* Called by one CPU */
void kernelMain(void) {

    using namespace VMM;

    // These should be noops
    naive_munmap((void*) kConfig.ioAPIC);
    naive_munmap((void*) kConfig.localAPIC);

    // Check for on demand paging by allocating more than physical space in memory
    auto ptr = (int*) naive_mmap(kConfig.memSize + 1,false,StrongPtr<Node>{},0);
    Debug::printf("*** Passed 1st section!\n");

    // Check that you only load specific pages from VMEs instead of the whole entry
    for (uint32_t i = 0; i < 1000; i ++) {
        ptr[i * PhysMem::FRAME_SIZE] = 'd'; // dummy values for the sake of loading pages in
    }
    naive_munmap(ptr);
    Debug::printf("*** Passed 2nd section!\n");

    // Check that you allocate to the same spot you unmmaped
    for (uint32_t i = 0; i < 1000; i ++ ) {
        naive_mmap(PhysMem::FRAME_SIZE, false, StrongPtr<Node>{}, 0);
    }

    for (uint32_t i = 0; i < 10000; i ++) {
        naive_munmap((void *)  + (0x80000000 + (i % 1000) * PhysMem::FRAME_SIZE));
        auto addr = naive_mmap(PhysMem::FRAME_SIZE, false, StrongPtr<Node>{}, 0);
        ASSERT((0x80000000 + (i % 1000) * PhysMem::FRAME_SIZE) == (uint32_t) addr);
    }
    Debug::printf("*** Passed 3rd section!\n");

    // Check that you flush your TLB
    for (uint32_t i = 0; i < 1000; i ++) {
        auto addr = (int *) naive_mmap(sizeof(int), false, StrongPtr<Node>{}, 0);
        *addr = 10;
        naive_munmap(addr);
        addr = (int *) naive_mmap(sizeof(int), false, StrongPtr<Node>{}, 0);
        ASSERT(*addr == 0);
    }
    Debug::printf("*** Passed 4th section!\n");

}

