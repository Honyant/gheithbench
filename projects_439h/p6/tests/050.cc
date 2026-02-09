#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"
#include "libk.h"
#include "barrier.h"

/* Called by one CPU */
void kernelMain(void) {
    using namespace VMM;
    //*** Functional Test Case***
    
    auto ide = StrongPtr<Ide>::make(1, 1);
    auto fs = StrongPtr<Ext2>::make(ide);
    auto root = fs->root;

    Debug::printf("*** Spell #1: Anonymous Mapping and Unmapping\n");
    // Checks mapping and unmapping
    void* spell1 = naive_mmap(0x1000, false, StrongPtr<Node>{}, 0);
    CHECK(spell1 == (void*)0x80000000);
    naive_munmap(spell1);

    Debug::printf("*** Spell #2: File Mapping\n");
    // Checks mapping and unmapping but for files
    auto fileNode = fs->find(root, "harrypotter1");
    void* spell2 = naive_mmap(fileNode->size_in_bytes(), false, fileNode, 0);
    CHECK(spell2 == (void*)0x80000000);
    Debug::printf("%s\n", spell2);
    naive_munmap(spell2);

    Debug::printf("*** Spell #3: Fragmentation and Reuse\n");
    // 4KB (taken) 8KB (taken)
    // 4KB (free) 8KB (taken)
    // 4KB (free) 8KB (taken) 8KB (taken)
    // 4KB (taken) 8KB (taken) 8KB (taken)
    void* spell3 = naive_mmap(0x1000, false, StrongPtr<Node>{}, 0); 
    void* spell4 = naive_mmap(0x2000, false, StrongPtr<Node>{}, 0); 
    CHECK(spell3 == (void*)0x80000000);
    CHECK(spell4 == (void*)0x80001000);
    naive_munmap(spell3);
    void* spell5 = naive_mmap(0x2000, false, StrongPtr<Node>{}, 0);
    CHECK(spell5 == (void*)0x80003000);
    void* spell5_2 = naive_mmap(0x1000, false, StrongPtr<Node>{}, 0);
    CHECK(spell5_2 == (void*)0x80000000);
    naive_munmap(spell4);
    naive_munmap(spell5);
    naive_munmap(spell5_2);
    

    Debug::printf("*** Spell #4: Multipage\n");
    // Large Map + offset unmap
    void* spell6 = naive_mmap(0x4000, false, StrongPtr<Node>{}, 0); 
    CHECK(spell6 == (void*)0x80000000);
    naive_munmap((void*)((uint32_t)spell6 + 0x2000));
    void* spell7 = naive_mmap(0x1000, false, StrongPtr<Node>{}, 0);
    CHECK(spell7 == (void*)0x80000000);
    naive_munmap(spell7);

    auto barrier = new Barrier(2);
    auto barrier2 = new Barrier(2);

    Debug::printf("*** Spell #5: Shared Memory\n");
    // Checks if you can access shared memory across threads
    void* shared_memory = naive_mmap(0x1000, true, StrongPtr<Node>{}, 0);
    CHECK(shared_memory == (void*)0xF0000000);
    ((char*)shared_memory)[0] = 'A';

    thread([shared_memory, barrier, barrier2] {
        barrier->sync();
        CHECK(((char*)shared_memory)[0] == 'A');
        barrier2->sync();
    });

    barrier->sync();
    barrier2->sync();
}
