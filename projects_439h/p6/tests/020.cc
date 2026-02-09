#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"

/* Called by one CPU */
/** This is a comprehensive test filled with things that should hopefully expose common bugs :) */
void kernelMain(void) {
    using namespace VMM;
    auto ide = StrongPtr<Ide>::make(1, 1);
    auto fs = StrongPtr<Ext2>::make(ide);
    auto root = fs->root;

    Debug::printf("*** Welcome to my test case!\n");

    // do you assume that the head of the list is non null when you unmap?
    Debug::printf("*** Testing unmapping iopic and localapic if there is nothing in the vme list\n");
    naive_munmap((void*)kConfig.ioAPIC);
    naive_munmap((void*)kConfig.localAPIC);

    // basic mmap and unmap with loading data in:
    Debug::printf("*** Testing a regular mmap, read, then unmap with a file passed in\n");
    auto file1 = fs->find(root, "file1");
    auto p = (char*) naive_mmap(file1->size_in_bytes(), false, file1, 0);
    CHECK(p == (char*) 0x80000000);
    Debug::printf("%s",p);
    naive_munmap((void*) p);

    // mapping and unmapping without loading the data in! (make sure to check the present bit for the pd)
    Debug::printf("*** Testing mmapping multiple pages without loading anything in if you lazy load\n");
    for (int i = 0; i < 20; i++) {
        auto p1 = (char*) naive_mmap(4097, false, StrongPtr<Node>{}, 0); // to make it more than one page
        CHECK(p1 == (char*) 0x80000000);
        naive_munmap((void*) p1);
    }

    // unmapping then re mapping test!
    // make sure to reset your present bit when you unmap so it page faults again!
    Debug::printf("*** Testing unmapping then remapping\n");
    for (int i = 0; i < 100; i++) { 
        auto p1 = (uint32_t*) naive_mmap(1, false, StrongPtr<Node>{}, 0);
        CHECK(p1 == (uint32_t*) 0x80000000);
        ASSERT(p1[0] == 0);
        p1[0] = 10;
        ASSERT(p1[0] == 10);
        naive_munmap((void*) p1);

        auto p2 = (uint32_t*) naive_mmap(1, false, StrongPtr<Node>{}, 0);
        ASSERT(p2 == (uint32_t*) 0x80000000);
        ASSERT(p2[0] == 0);
        naive_munmap((void*) p2);
    }

    // unmap tests!
    // If you pass the first one but fail the second one, make sure you're checking when you iterate and not just align the pointer to a page

    // testing unmapping within one page
    Debug::printf("*** Unmapping an unaligned pointer that is within one page\n");
    auto unmapUnalignedInPage = (uint32_t*) naive_mmap(0x1000, false, StrongPtr<Node>{}, 0); // mapping one page
    CHECK(unmapUnalignedInPage == (uint32_t*) 0x80000000);
    naive_munmap((void*) ((uint32_t) unmapUnalignedInPage + 0x0001));
    auto afterInPage = (uint32_t*) naive_mmap(1, false, StrongPtr<Node>{}, 0);
    CHECK(afterInPage == (uint32_t*) 0x80000000);
    naive_munmap((void*) afterInPage);

    // testing unmapping between two pages
    Debug::printf("*** Unmapping an unaligned pointer that is not on the first page\n");
    auto unmapUnalignedAcrossPage = (uint32_t*) naive_mmap(0x1999, false, StrongPtr<Node>{}, 0); // mapping two pages
    CHECK(unmapUnalignedAcrossPage == (uint32_t*) 0x80000000);
    naive_munmap((void*) ((uint32_t) unmapUnalignedAcrossPage + 0x1001));
    auto afterInPage2 = (uint32_t*) naive_mmap(1, false, StrongPtr<Node>{}, 0);
    CHECK(afterInPage2 == (uint32_t*) 0x80000000);
    naive_munmap((void*) afterInPage2);


    // Round 2: where we actually load in the memory
    // testing unmapping within one page
    Debug::printf("*** Unmapping an unaligned pointer that is within one page when we actually load something in\n");
    auto unmapUnalignedInPageLoadIn = (char*) naive_mmap(0x1000, false, file1, 0); // mapping one page
    CHECK(unmapUnalignedInPageLoadIn == (char*) 0x80000000);
    Debug::printf("%s", unmapUnalignedInPageLoadIn);
    naive_munmap((void*) ((uint32_t) unmapUnalignedInPageLoadIn + 0x0001));
    auto afterInPageLoadIn = (uint32_t*) naive_mmap(1, false, StrongPtr<Node>{}, 0);
    CHECK(afterInPageLoadIn == (uint32_t*) 0x80000000);
    naive_munmap((void*) afterInPageLoadIn);

    // testing unmapping between two pages
    Debug::printf("*** Unmapping an unaligned pointer that is not on the first page when we actually load something in\n");
    auto unmapUnalignedAcrossPageLoadIn = (char*) naive_mmap(0x1999, false, file1, 0); // mapping two pages
    CHECK(unmapUnalignedAcrossPageLoadIn == (char*) 0x80000000);
    Debug::printf("%s", unmapUnalignedAcrossPageLoadIn);
    naive_munmap((void*) ((uint32_t) unmapUnalignedAcrossPageLoadIn + 0x1001));
    auto afterInPageLoadIn2 = (uint32_t*) naive_mmap(1, false, StrongPtr<Node>{}, 0);
    CHECK(afterInPageLoadIn2 == (uint32_t*) 0x80000000);
    naive_munmap((void*) afterInPageLoadIn2);
}
