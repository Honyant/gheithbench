#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"

/*
 * this test cases thoroughly tests insertion to vme
 * to make sure that insertion is tightly packed
 */

/* Called by one CPU */
void kernelMain(void)
{

    using namespace VMM;
    auto ide = StrongPtr<Ide>::make(1, 1);
    auto fs = StrongPtr<Ext2>::make(ide);
    auto root = fs->root;

    auto uwu = fs->find(root, "uwu");
    // 0x80000000 to 0xF0000000 in the virtual memory space is private to each thread

    // maps 0x80000000 to 0x80000999
    auto p = (char *)naive_mmap(uwu->size_in_bytes(), false, uwu, 0);
    CHECK(p == (char *)0x80000000);
    Debug::printf("*** 1\n");
    Debug::printf("%s\n", p);
    Debug::printf("*** Check 1: you can mmap and print file\n");

    // maps 0x80001000 to 0x80003999
    auto q = (char *)naive_mmap(uwu->size_in_bytes() + 2 * PhysMem::FRAME_SIZE, false, uwu, 0);
    CHECK(q == (char *)0x80001000);
    Debug::printf("*** 2\n");
    Debug::printf("%s\n", q);
    Debug::printf("*** Check 2: you can mmap and print file\n");

    // maps 0x80004000 to 0xE9999999
    auto r = (char *)naive_mmap(uwu->size_in_bytes() + 0x6FFFB000, false, uwu, 0); 
    CHECK(r == (char *)0x80004000);
    Debug::printf("*** 3\n");
    Debug::printf("%s\n", r);
    Debug::printf("*** Check 3: you can mmap and print a really large file\n");

    // attemps to map, but virtual address space is full
    auto s = (char *)naive_mmap(1, false, StrongPtr<Node>{}, 0);
    CHECK(s == nullptr);
    Debug::printf("*** 4\n");
    Debug::printf("*** Check 4: you returned nullptr when virtual address space is full\n");

    // unmap
    naive_munmap((void *)q);
    Debug::printf("*** 5\n");
    Debug::printf("*** Check 5: you can unmap\n");

    // maps 0x80001000 to 0x80001999
    auto t = (char *)naive_mmap(uwu->size_in_bytes(), false, uwu, 0);
    CHECK(t == (char *)0x80001000);
    Debug::printf("*** 6.0\n");
    Debug::printf("%s\n", t);

    // maps 0x80002000 to 0x80003999
    auto u = (char *)naive_mmap(uwu->size_in_bytes() + PhysMem::FRAME_SIZE, false, uwu, 0);
    CHECK(u == (char *)0x80002000);
    Debug::printf("*** 6.1\n");

    Debug::printf("%s\n", u);
    Debug::printf("%s\n", r);
    Debug::printf("*** Check 6: you can mmap in space you just unmapped\n");

    // unmaps 0x80002500 - in the middle of a page
    naive_munmap((void *)0x80002500);
    CHECK(1 == 1); // placeholder check
    Debug::printf("*** 7\n");
    Debug::printf("*** Check 7: you can unmap in the middle of a page\n");

    // unmaps 0x80002000
    auto w = (char *)naive_mmap(PhysMem::FRAME_SIZE, false, uwu, 0);
    Debug::printf("*** 8\n");
    CHECK(w == (char *)0x80002000);
    Debug::printf("%s\n", w);
    Debug::printf("*** Check 8: when unmapping in the middle of a page, you unmapped\n");
    Debug::printf("*** the whole page and you can mmap again at the beginning of the page\n");
}
