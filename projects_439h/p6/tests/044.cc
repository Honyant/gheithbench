#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"
#include "atomic.h"
#include "barrier.h"
#include "vmm.h"

using namespace VMM;

void show(const char* va, uint32_t size, uint32_t increment_size, bool all = false) {
    if (va == nullptr) {
        Debug::printf("***      does not exist\n");
        return;
    }

    Debug::printf("*** ");
    for (size_t i = 0; i < size; i += increment_size) {
        if ((va[i] > 0 && va[i] < 255) || all) {
            Debug::printf("%c", va[i]);
            if (va[i] == '\n') Debug::printf("*** ");
        }
    }
    Debug::printf("\n");
}

/* Called by one core */
void kernelMain(void) {
    const uint32_t PAGE_SIZE = PhysMem::FRAME_SIZE;

    // IDE device #1
    auto ide = StrongPtr<Ide>::make(1, 3);
    
    // We expect to find an ext2 file system there
    auto fs = StrongPtr<Ext2>::make(ide);
    auto root = fs->root;

    auto mousefile = fs->find(root,"mouse.txt");
    auto catfile = fs->find(root,"cat.txt");

    // get file from root (0)
    auto file0 = fs->find(root,"0");
    auto file1 = fs->find(root,"1");
    auto file2 = fs->find(root,"2");


    /*************************
     * 
     * test 0: a simple mmap
     * 
     *************************/

    Debug::printf("*** test 0: basic mmap functionality\n");

    const uint32_t size0 = file0->size_in_bytes();
    char* va0 = (char*) naive_mmap(size0, false, file0, 0);
    CHECK(va0 == (char*) 0x80000000);
    show(va0, size0, 1);

    va0[0] = '!';
    va0[1] = '!';
    va0[2] = '!';
    va0[3] = '!';

    show(va0, size0, 1);

    Debug::printf("\n");

    /*************************
     * 
     * test 1: basic mmap, outside of filesize
     * 
     *************************/

    Debug::printf("*** test 1: mmap one page\n");

    const uint32_t size1 = file1->size_in_bytes();
    char* va1 = (char*) naive_mmap(size1, false, file1, 0);
    CHECK(va1 == (char*) 0x80001000);
    show(va1, size1, 1);

    // should be able to write to the last byte of page
    va1[PAGE_SIZE-4] = 'h';
    va1[PAGE_SIZE-3] = 'i';
    va1[PAGE_SIZE-2] = 's';
    va1[PAGE_SIZE-1] = 's';

    show(va1, PAGE_SIZE, 1);

    // can uncomment, should panic
    // va1[PAGE_SIZE] = 'w';

    Debug::printf("\n");

    /*************************
     * 
     * test 2: mmap multiple pages
     * 
     *************************/

    
    Debug::printf("*** test 2: mmap multiple pages\n");

    char* mouse = (char*) naive_mmap(mousefile->size_in_bytes(), false, mousefile, 0);
    CHECK(mouse == (char*) 0x80002000);
    show(mouse, mousefile->size_in_bytes(), 1, true);

    const uint32_t size2 = file2->size_in_bytes();
    char* va2 = (char*) naive_mmap(PAGE_SIZE + 1, false, file2, 0);
    CHECK(va2 == (char*) 0x80003000);
    show(va2, size2, 1);

    // should be able to write to second page
    for (uint32_t i = 0; i < mousefile->size_in_bytes(); ++i) {
        va2[PAGE_SIZE + i] = mouse[i];
    }
    show(va2, PAGE_SIZE*2, 1);

    Debug::printf("\n");

    /*************************
     * 
     * test 3: you can write to multiple private pages.
     * let's test lazy loading and sparse writing in all private memory.
     * 
     *************************/

    Debug::printf("*** test 3: mmap fatcat boundaries of private memory\n");

    const uint32_t PRIVATE_LIMIT = 0xF0000000; 
    const uint32_t CAT_ADDR = 0x80005000;
    const uint32_t FATCAT_SIZE = PRIVATE_LIMIT -  CAT_ADDR;
    const uint32_t PAGES_AT_A_TIME = 1000;

    char* cat = (char*) naive_mmap(FATCAT_SIZE, false, catfile, 0);
    CHECK((uint32_t) cat == CAT_ADDR);
    show(cat, catfile->size_in_bytes(), 1);

    // should be able to write a char to any page in this range
    for (uint32_t i = CAT_ADDR; i < PRIVATE_LIMIT; i += PAGE_SIZE * PAGES_AT_A_TIME) {
        uint32_t write_to_byte = i - CAT_ADDR;
        cat[write_to_byte] = '*';
        // Debug::printf("*** wrote to %x\n", i);
    }
    cat[FATCAT_SIZE - 1] = '*';
    Debug::printf("*** wrote to last byte, %x\n", FATCAT_SIZE - 1 + CAT_ADDR);
    show(cat, FATCAT_SIZE, PAGE_SIZE * PAGES_AT_A_TIME * 10);

    Debug::printf("\n");

    /*************************
     * 
     * test 4: let's unmap pages we didn't load
     * 
     *************************/ 

    Debug::printf("*** test 4: unmap fatcat pages we didn't load\n");

    const int OFFSET = 0x1000;
    for (uint32_t i = CAT_ADDR + OFFSET; i < PRIVATE_LIMIT; i += PAGE_SIZE * PAGES_AT_A_TIME) {
        naive_munmap((void*) (CAT_ADDR + i));
    }
    show(cat, FATCAT_SIZE, PAGE_SIZE * PAGES_AT_A_TIME * 10);
 
    Debug::printf("\n");

    /*************************
     * 
     * test 5: let's unmap all the fatcat pages we mapped
     * 
     *************************/ 

    Debug::printf("*** test 5: unmap all fatcat pages\n");

    for (uint32_t i = CAT_ADDR; i < PRIVATE_LIMIT; i += PAGE_SIZE * PAGES_AT_A_TIME) {
        naive_munmap((void*) i);
    }
    uint32_t va_check = (uint32_t) naive_mmap(1, false, StrongPtr<Node>{}, 0);
    CHECK(va_check == 0x80005000);

}

