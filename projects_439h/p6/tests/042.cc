#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"
#include "physmem.h"
#include "vmm.h"
#include "barrier.h"
#include "atomic.h"

void kernelMain(void) {
    using namespace VMM;

    uint32_t num_iterations = 1000;
    
    // can you turn on VM?
    Debug::printf("*** Virtual Memory is now on!\n"); 

    auto ide = StrongPtr<Ide>::make(1, 1);
    auto fs = StrongPtr<Ext2>::make(ide);
    auto root = fs->root;
    auto file1 = fs->find(root,"file1.txt");
    auto file2 = fs->find(root,"/dir1a/dir1b/file4.txt");
    auto file3 = fs->find(root,"/dir1a/dir1b/dir1d/file6.txt");

    // Stress testing mmap and unmaps to ensure the memory is actually deallocated right after unmap (if it isn't, it will not finish the iterations because program will run out of frames). Also ensuring that the program can read/write to mmapped memory
    for (uint32_t i = 0; i < num_iterations; i++) {
        char* f1_va = (char*) naive_mmap(file1->size_in_bytes() * 100 * PhysMem::FRAME_SIZE, false, file1, 0);
        for (uint32_t j = 0; j < 100; j++) {
            f1_va[PhysMem::FRAME_SIZE * j] = 'x'; // allocating to each page
        }

        CHECK(f1_va == (char*) 0x80000000); // ensuring mmap returns lowest possible address
        char* f2_va = (char*) naive_mmap(file2->size_in_bytes(), false, file2, 0);
        char* f3_va = (char*) naive_mmap(file3->size_in_bytes(), false, file3, 0);
        if (i % 200 == 0) {
            f1_va[3] = 'h';
            char old_char = f1_va[3];
            f1_va[3] = old_char + (i / 200);
            Debug::printf("*** On iteration: %d; You wrote: %c to replace: %c\n", i, (char) f1_va[3], old_char);
        }
        naive_munmap(f1_va);
        naive_munmap(f2_va);
        naive_munmap(f3_va);
    }
    Debug::printf("*** Done with mmap/unmap stress test!\n");

    // Testing mmaping in shared memory with multiple threads to test synchronization of shared memory reading/writing and ensure changes in memory are shown in other threads as well
    Barrier *thread_bar = new Barrier(num_iterations / 10 + 1);
    char* f2_va = (char*) naive_mmap(file2->size_in_bytes(), true, file2, 0);
    f2_va[3] = 'a';
    for (uint32_t i = 0; i < num_iterations / 10; i++) {
        if (i % 2 == 0) {
            thread([f2_va, thread_bar] {
                f2_va[3] = 'x';
                thread_bar->sync();
            });
        } else {
            thread([f2_va, thread_bar] {
                f2_va[3] = 'x';
                thread_bar->sync();
            });
        }
    }    
    thread_bar->sync();
    Debug::printf("*** new value of the f2_va character should be 'x': %c\n", (char) f2_va[3]);
    Debug::printf("*** Done with shared memory test!\n");

}

