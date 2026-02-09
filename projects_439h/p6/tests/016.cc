#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "barrier.h"

/* 
    The following test is meant to ensure that shared memory works as expected.
*/

const int N = 10;

void kernelMain(void) {
    using namespace VMM;

    auto ide = StrongPtr<Ide>::make(1, 0);
    auto fs = StrongPtr<Ext2>::make(ide);
    auto root = fs->root;

    // Test shared read/write
    int* p = (int*)naive_mmap(1, true, StrongPtr<Node>{}, 0); // allocate a shared page
    ASSERT((uint32_t)p == 0xF0000000);
    for (int i = 1; i <= N; i++) {
        thread([p, i] {
            while (p[0] != i - 1) {
                yield();
            }
            p[0] = i;
        });
    }
    while (p[0] != N) {
        yield();
    }
    Debug::printf("*** Yay we can read/write to shared pages!\n");

    // Test accessing shared memory that didn't exist at thread creation
    StrongPtr<Barrier> b1{new Barrier(2)};
    StrongPtr<Barrier> b2{new Barrier(2)};
    thread([b1, b2]() mutable {
        b1->sync();
        char* p = (char*)(0xF0001000); // this is where the shared page should be
        Debug::printf("%s\n", p); // Successfully accessed shared memory that was mapped after thread creation!
        b2->sync();
    });
    StrongPtr<Node> msg1 = fs->find(root, "msg1.txt");
    p = (int*)naive_mmap(msg1->size_in_bytes(), true, msg1, 0); // allocate a shared page
    ASSERT((uint32_t)p == 0xF0001000);
    b1->sync();
    b2->sync();


    // Let's make sure we don't overwrite localApic and ioApic
    int size = 0xFEC00000 - 0xF0002000; // size to reach localAPIC
    auto p1 = naive_mmap(size + 1, true, StrongPtr<Node>{}, 0);
    ASSERT((uint32_t)p1 == 0); // can't fit below the localAPIC (must be nullptr)
    auto p2 = naive_mmap(size, true, StrongPtr<Node>{}, 0);
    ASSERT((uint32_t)p2 == 0xF0002000); // fits just below the localAPIC
    auto p3 = naive_mmap(15, true, StrongPtr<Node>{}, 0);
    ASSERT((uint32_t)p3 == 0xFEC01000); // fits between localAPIC and ioAPIC
    Debug::printf("*** We didn't overwrite our localAPIC and ioAPIC!\n");



    auto hello = fs->find(root,"hello");

}
