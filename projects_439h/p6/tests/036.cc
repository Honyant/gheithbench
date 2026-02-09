// Page table freeing test

#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "barrier.h"
#include "physmem.h"
#include "promise.h"

constexpr uint32_t N = 50000;
constexpr uint32_t VMM_FRAMES_START = 6 * 1024 * 1024;
uint32_t NUM_VMM_FRAMES = (kConfig.memSize - VMM_FRAMES_START) / PhysMem::FRAME_SIZE; // ~30,000 frames total available to get allocated
constexpr uint32_t alloc_size = 0xf0000000 - 0x80000000; // = 0x70000000
constexpr uint32_t pd_size = PhysMem::FRAME_SIZE * PhysMem::FRAME_SIZE / sizeof(uint32_t); // = 0x00100000
uint32_t numIterations = (NUM_VMM_FRAMES / (alloc_size/pd_size)) + 1; // = ~30,000/448 + 1 = 70

void kernelMain(void) {
    using namespace VMM;

    // Easier Bonus Test: Spawn threads without mmapping to ensure you delete your data structures to store VMEs and page directories

    // If you can uncommment the code block below and reach the main test (finish 50,000 iterations), that means you 
    // delete/free thread data correctly. This was not a part of the main test case because the main test is much more time-consuming

    // for (uint32_t i = 0; i < N; i++) {
    //     thread([i]{
    //         if (i % 1000 == 0) {
    //             Debug::printf("Reached %uth iteration\n", i);
    //         }
    //     });
    // }

    // Main Test: Test for page table freeing -> avoiding memory leaks

    // I map the full private address space for a thread by the size each page
    // table corresponds to so that you cannot shortcut it by reusing the same
    // page again and again.

    // We want to enforce an ordering of threads since we want to guarantee
    // unmappings to occur before new mappings are made. For debugging purposes
    // ignore the synchronization primitives, since they only force the page table
    // allocation

    ASSERT(numIterations == 70);

    Promise<bool> *promiseOrder = new Promise<bool>[numIterations];
    Barrier *b = new Barrier(numIterations+1);

    //Allow 4 threads to run at the same time
    for (uint32_t i = 0; i < kConfig.totalProcs; i++) {
        promiseOrder[i].set(true);
    }

    for (uint32_t i = 1; i <= numIterations; i++) {
        thread([i , &promiseOrder,&b] {
            //Resume thread once it gets its turn
            promiseOrder[i - 1].get();

            for (uint32_t j = 0; j < alloc_size/pd_size; j++) {
                //allocate the size of a page table
                char* p = (char*) naive_mmap(pd_size, false, {}, 0);
                //Debug::printf("Mapping address %x\n", p);

                //Force a page mapping, thus the creation of a page table as well
                p[0] = '\0';  
            }

            char* p = (char*) 0x80000000; //start address

            for (uint32_t j = 0; j < alloc_size/pd_size; j++) {
                //Force unmapping, this is where you should also free the page table
                //Debug::printf("Unmapping address %x\n", p);
                naive_munmap((void*)p);
                p += pd_size;
            }

            if (i <= numIterations - kConfig.totalProcs) {
                //release another thread
                promiseOrder[i + 3].set(true);
            }
            b->sync();
        });
    }
    b->sync();

    Debug::printf("*** Passed Test. Good job on freeing page tables whenever possible\n");
}