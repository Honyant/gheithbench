#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"
#include "barrier.h"


void kernelMain(void) {

    using namespace VMM;

    auto ide = StrongPtr<Ide>::make(1, 1);
    
    auto fs = StrongPtr<Ext2>::make(ide);
   
    auto root = fs->root;

    auto info = fs->find(root,"info.txt");

    char** mapped_pointers = new char*[info->size_in_bytes()];

    //testing that mmap returns the lowest possible virtual address by mmapping sequentially and verifying returned address
    //this also tests that we mmap space up to the nearest page because a single page should be mmapped each time.
    for (uint32_t i = 0; i < info->size_in_bytes(); i++) {
        //tests the offset argument to the mmap function by allocating with a different offset each iteration
        auto p = (char*) naive_mmap(info->size_in_bytes(),false,info,i);
        mapped_pointers[i] = p;
        CHECK(p == (char*) (0x80000000 + i * PhysMem::FRAME_SIZE));
        Debug::printf("*** %s",p);
    }

    for (uint32_t i = 0; i < info->size_in_bytes(); i++) {
        CHECK(mapped_pointers[i] == (char*) (0x80000000 + i * PhysMem::FRAME_SIZE));
    }

    //we unmap the 10th allocated page from the start of the private address space
    //verifies that we return the lowest possible virtual address
    naive_munmap(mapped_pointers[10]);

    //the next attempt to mmap a page should return the virtual address at the location we just unmapped because there is
    //one page of space there
    auto p = (char*) naive_mmap(info->size_in_bytes(),false,info,0);
    CHECK(p == (char*) (0x80000000 + 10 * PhysMem::FRAME_SIZE));

    //creating 1 page of space at virtual address 0x80000000 + 10 * PhysMem::FRAME_SIZE
    naive_munmap(p);

    //we unmap two consecutive pages
    naive_munmap(mapped_pointers[15]);
    naive_munmap(mapped_pointers[16]);

    //an attempt to mmap two pages should return the virtual address starting at the recently unmapped region which 
    //has space for two pages
    //we should not mmap to the virtual address unmapped in line 49 since there is only space for 1 page there
    //confirms that mmap allocates up to the nearest page size requested
    p = (char*) naive_mmap(info->size_in_bytes()+PhysMem::FRAME_SIZE,false,info,0);
    CHECK(p == (char*) (0x80000000 + 15 * PhysMem::FRAME_SIZE));

    //test that we return nullptr if we do not have space for the mmap
    p = (char*) naive_mmap(0x70000*PhysMem::FRAME_SIZE,false,info,0);
    CHECK(p == nullptr);

    //testing that each thread has its own virtual address space
    //the attempt to mmap should allocate space at the start of the virtual address space even though that virtual address
    //has already been mmapped to since we are on a new thread
    Barrier* b1 = new Barrier(2);
    thread([b1] {
        auto ide = StrongPtr<Ide>::make(1, 1);
        auto fs = StrongPtr<Ext2>::make(ide);
        auto root = fs->root;
        auto info = fs->find(root,"info.txt");
        for (uint32_t i = 0; i < info->size_in_bytes(); i++) {
            //we repeatedly mmap and unmap, so each mmap should be allocated at the same virtual address. The virtual address
            //of the allocation should not increment.
            auto p = (char*) naive_mmap(info->size_in_bytes(),false,info,0);
            CHECK(p == (char*) (0x80000000));
            //tests that munmap frees this virtual address to be remapped during the next iteration of the loop
            naive_munmap(p);
        }
        b1->sync();
    });
    b1->sync();

    auto shared = fs->find(root,"shared.txt");

    //tests that each thread can correctly access the shared address space

    //let's mmap into the shared address space
    p = (char*) naive_mmap(info->size_in_bytes(),true,shared,0);
    CHECK(p == (char*) (0xF0000000));

    //each of these threads should correctly find the shared memory
    Barrier* b2 = new Barrier(2);
    thread([b2] {
        Debug::printf("*** %s",(char*) (0xF0000000));
        b2->sync();
    });
    b2->sync();

    Barrier* b3 = new Barrier(2);
    thread([b3] {
        Debug::printf("*** %s",(char*) (0xF0000000));
        b3->sync();
    });
    b3->sync();


   
}
