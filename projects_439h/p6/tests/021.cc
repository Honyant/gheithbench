#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "barrier.h"

/* Called by one CPU */
void kernelMain(void) {
    using namespace VMM;

    auto ide = StrongPtr<Ide>::make(1, 1);
    auto fs = StrongPtr<Ext2>::make(ide);   
    auto root = fs->root;

    // testing a simple mmap to private memory space
    Debug::printf("*** Test #1: a regular mmap to private memory space\n");
    auto simpleNode = fs->find(root,"simple.txt");
    auto simplePtr = (char*) naive_mmap(simpleNode->size_in_bytes(),false,simpleNode,0);
    Debug::printf("*** %s\n", simplePtr);

    // testing mapping to shared memory
    Debug::printf("*** Test #2: mapping to shared memory\n");
    auto sharedNode = fs->find(root,"shared.txt");
    auto sharedPtr = (char*) naive_mmap(sharedNode->size_in_bytes(),true,sharedNode,0);
    // 0xF0000000 - 0xFFFFFFFF+1
    ASSERT((uint32_t) sharedPtr >= 0xF0000000 && (uint32_t) sharedPtr <= 0xFFFFFFFF);
    Debug::printf("*** %s\n", sharedPtr);

    // testing that unmapping shared memory doesn't do anything
    Debug::printf("*** Test #3: cannot unmap shared memory\n");
    naive_munmap(sharedPtr);
    Debug::printf("*** If you handle this incorrectly, then the next test will fail.\n");

    // testing that threads can all access the shared memory
    Debug::printf("*** Test #4: shared memory can be accessed by all threads\n");
    uint32_t num_threads = 10;
    auto barrier = new Barrier(num_threads + 1);
    for (uint32_t i = 0; i < num_threads; i++) {
        thread([barrier, sharedPtr] {
            Debug::printf("*** %s\n", sharedPtr);
            barrier->sync();
        });
    }
    barrier->sync();

    // testing that files are always mapped to the first available location in memory
    Debug::printf("*** Test #5: files are mapped to first available location in VM\n");
    auto ptr1 = (char*) naive_mmap(100,false, StrongPtr<Node>{},0);
    auto ptr2 = (char*) naive_mmap(100,false, StrongPtr<Node>{},0);
    auto ptr3 = (char*) naive_mmap(100,false, StrongPtr<Node>{},0);
    uint32_t ptr2_num = (uint32_t) ptr2;
    naive_munmap(ptr2);
    auto locNode = fs->find(root,"linked.txt");
    auto locPtr = (char*) naive_mmap(locNode->size_in_bytes(),false,locNode,0);
    ASSERT(ptr1 != ptr3);
    ASSERT((uint32_t) locPtr == ptr2_num);
    Debug::printf("*** %s\n", (char*) ptr2);
}
