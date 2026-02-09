#include "shared.h"
#include "threads.h"
#include "barrier.h"
#include "debug.h"
#include "ext2.h"
#include "ide.h"
#include "vmm.h"

void kernelMain(void) {

    Debug::printf("*** You can turn on virtual memory!\n");
    
    using namespace VMM;

    auto ide = StrongPtr<Ide>::make(1, 1);
    auto fs = StrongPtr<Ext2>::make(ide);
    
    auto root = fs->root;
    auto goats = fs->find(root, "goats.txt");
    auto legends = fs->find(root, "legends.txt");

    // Are you invalidating your TLB?
    auto p1 = (char*)naive_mmap(goats->size_in_bytes(), false, goats, 0);
    Debug::printf("*** First letter: %c\n", p1[0]);
    char x = p1[0];
    naive_munmap(p1);

    auto p2 = (char*)naive_mmap(legends->size_in_bytes(), false, legends, 0);
    Debug::printf("*** First letter: %c\n", p2[0]);
    CHECK(p1 == p2); // The same virtual address
    CHECK(x != p2[0]); // Should have different data

    naive_munmap(p2);
    
    auto shared_p1 = (char*)naive_mmap(goats->size_in_bytes(), true, goats, 0);
    auto shared_p2 = (char*)naive_mmap(legends->size_in_bytes(), true, legends, 0);

    CHECK(shared_p1[0] != shared_p2[0]);
    CHECK(shared_p1[0] == '*');
    CHECK(shared_p2[0] == 'J');

    x = shared_p1[0];

    // Let's create a bunch of threads and check if shared pages have the same values when updated
    StrongPtr<Barrier> b1 = StrongPtr<Barrier>::make(100);
    StrongPtr<Barrier> b2 = StrongPtr<Barrier>::make(100);
    StrongPtr<Barrier> b3 = StrongPtr<Barrier>::make(101);
    
    for (int i = 0; i < 100; i++) {
        thread([b1, b2, b3, shared_p1, i, x]() {
            CHECK(shared_p1[0] == x);
            b1->sync(); // check that we all have the same initial value
            if (i == 0) shared_p1[0] = 'Z';
            b2->sync(); // check that we all have the same updated value
            
            CHECK(shared_p1[0] == 'Z');
            
            // Your unmap should do nothing for shared pages
            naive_munmap(shared_p1);
            b3->sync(); // check that unmapping does nothing
        });
    }

    b3->sync();
    CHECK(shared_p1[0] == 'Z');

    // Try to get to the end of private virtual memory, if you don't have a working free list or if you spill into the shared section, you will fail here
    // This also stresses your speed
    uint32_t alloc_size = 1 << 16;
    uint32_t num_iters = (0xF0000000 - 0x80000000) / alloc_size + 3;

    for (uint32_t i = 0; i < num_iters; i++) {
        auto p = (char*)naive_mmap(alloc_size, false, goats, 0);
        CHECK(p >= (char*)0x80000000);
        CHECK(p <= (char*)0xF0000000);
        naive_munmap(p);
    }

    // End TC
    Debug::printf("*** You've made it to the end!\n");
}