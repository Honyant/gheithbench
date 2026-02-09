#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"


/*
This testcases tests the condition that your mmap always uses
the lowest available virtual address and that unmap is working
*/

/* Called by one CPU */
void kernelMain(void) {

    using namespace VMM;

    // IDE device #1
    auto ide = StrongPtr<Ide>::make(1, 1);
    // We expect to find an ext2 file system there
    auto fs = StrongPtr<Ext2>::make(ide);
   
    // get root
    auto root = fs->root;
    // get "hello"
    auto hello = fs->find(root,"hello");

    // make sure file system stuff is working
    CHECK(hello->is_file());
    CHECK(hello->size_in_bytes() < 4096);

    // this should use up a page (4096 bytes)
    auto a = (char*) naive_mmap(hello->size_in_bytes(),false,hello,0);
    CHECK(a == (char*) 0x80000000);
    // print the file to check content is correct    
    Debug::printf("%s\n",a);

    char* pages[10];
    pages[0] = a;
    // create 9 other page mappings
    for (int i = 1; i < 10; i ++) {
        pages[i] = (char*) naive_mmap(hello->size_in_bytes(),false,hello,0);
        // make sure lowest address is always used
        CHECK(pages[i] == (char*) (0x80000000 + i*0x1000));
    }

    // unmap some sections
    for(int i = 1; i < 3; i ++) { // page 1,2
        naive_munmap(pages[i]);
    }
    for(int i = 5; i < 8; i ++) { // page 5,6,7
        naive_munmap(pages[i]);
    }

    a = (char*) naive_mmap(hello->size_in_bytes() + 2*4096,false,hello,0);
    CHECK(a == (char*) 0x80005000); // should use up 5,6,7

    a = (char*) naive_mmap(hello->size_in_bytes() + 2*4096,false,hello,0);
    CHECK(a == (char*) 0x8000a000); // no space in previous, should start at page 10

    a = (char*) naive_mmap(hello->size_in_bytes() + 4096,false,hello,0);
    CHECK(a == (char*) 0x80001000); // should use up page 1,2

    Debug::printf("*** Done!\n");
}

