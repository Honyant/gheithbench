#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"
#include "barrier.h"

auto p7 = (char*) 0x80000000;
/* Called by one CPU */
void kernelMain(void) {
    using namespace VMM;
    // IDE device #1
    auto ide = StrongPtr<Ide>::make(1, 1);
    
    // We expect to find an ext2 file system there
    auto fs = StrongPtr<Ext2>::make(ide);
    auto root = fs->root;

    auto one = fs->find(root, "one");
    auto two = fs->find(root, "two");
    auto three = fs->find(root, "three");

    Debug::printf("*** Basic Map Check\n");
    auto p1 = (char*) naive_mmap(one->size_in_bytes(),false,one,0);
    ASSERT(p1 == (char*) 0x80000000);
    Debug::printf("%s",p1);

    auto p2 = (char*) naive_mmap(two->size_in_bytes(),false,two,0);
    ASSERT(p2 == (char*) 0x80001000);
    Debug::printf("%s",p2);

    auto p3 = (char*) naive_mmap(three->size_in_bytes(),false,three,0);
    ASSERT(p3 == (char*) 0x80002000);
    Debug::printf("%s",p3);

    Debug::printf("*** Basic Unmap Check\n");
    naive_munmap(p2);
    naive_munmap(p3);

    // if we reallocate three, it should be in the second page
    auto p4 = (char*) naive_mmap(three->size_in_bytes(),false,three,0);
    ASSERT(p4 == (char*) 0x80001000);
    naive_munmap(p1);
    naive_munmap(p4);

    // everything should be unmapped

    Debug::printf("*** Interleaved Map and Unmap Check\n");

    auto pa1 = (char*) naive_mmap(one->size_in_bytes(),false,one,0);
    auto pa2 = (char*) naive_mmap(one->size_in_bytes(),false,one,0);
    auto pa3 = (char*) naive_mmap(one->size_in_bytes(),false,one,0);
    auto pa4 = (char*) naive_mmap(one->size_in_bytes(),false,one,0);
    auto pa5 = (char*) naive_mmap(one->size_in_bytes(),false,one,0);
    auto pa6 = (char*) naive_mmap(one->size_in_bytes(),false,one,0);
    auto pa7 = (char*) naive_mmap(one->size_in_bytes(),false,one,0);
    auto pa8 = (char*) naive_mmap(one->size_in_bytes(),false,one,0);

    ASSERT(pa1 == (char*) 0x80000000);
    ASSERT(pa2 == (char*) 0x80001000);
    ASSERT(pa3 == (char*) 0x80002000);
    ASSERT(pa4 == (char*) 0x80003000);
    ASSERT(pa5 == (char*) 0x80004000);
    ASSERT(pa6 == (char*) 0x80005000);
    ASSERT(pa7 == (char*) 0x80006000);
    ASSERT(pa8 == (char*) 0x80007000);

    char* addresses[8] = {pa1, pa2, pa3, pa4, pa5, pa6, pa7, pa8};

    // Testing that unmapped memory regions are correctly reused by sequential remapping at the original addresses
    for (int i = 0; i < 8; i++) {
        naive_munmap(addresses[i]);

        char* new_pa = (char*) naive_mmap(one->size_in_bytes(), false, one, 0);
        ASSERT(new_pa == addresses[i]);
    }

    naive_munmap(pa1);
    naive_munmap(pa2);
    naive_munmap(pa3);
    naive_munmap(pa4);
    naive_munmap(pa5);
    naive_munmap(pa6);
    naive_munmap(pa7);
    naive_munmap(pa8);

    // testing that mappings smaller than a page are allocated at page-aligned addresses and correctly reuse addresses sequentially after unmapping
    Debug::printf("*** Page Size Check\n");
    for (uint32_t siz = 1; siz <= 4000; siz++) {
        auto p5 = (char*) naive_mmap(siz,false,StrongPtr<Node>{},0);
        ASSERT(p5 == (char*) 0x80000000);

        auto p6 = (char*) naive_mmap(siz,false,StrongPtr<Node>{},0);
        ASSERT(p6 == (char*) 0x80001000);

        naive_munmap(p5);
        naive_munmap(p6);
    }
    
    Debug::printf("*** Shared Memory Check\n");
    uint32_t num_threads = 5;
    auto sharedPtr = (char*) naive_mmap(one->size_in_bytes(), true, one, 0);

    // tests location of shared memory
    ASSERT((uint32_t) sharedPtr >= 0xF0000000 && (uint32_t) sharedPtr <= 0xFFFFFFFF);

    // tests if multiple threads can access shared memory
    auto barrier = new Barrier(num_threads + 1);
    for (uint32_t i = 0; i < num_threads; i++) {
        thread([barrier, sharedPtr] {
            Debug::printf("%s", sharedPtr);
            barrier->sync();
        });
    }
    barrier->sync();
}
