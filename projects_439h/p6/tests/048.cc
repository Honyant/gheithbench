#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"
#include "barrier.h"

char* privateStart(StrongPtr<Node> file) {
    using namespace VMM;
    auto p = (char*) naive_mmap(file->size_in_bytes(),false,file,0);
    auto cpy = p;
    naive_munmap(p);
    return cpy;
}

//File1 has 100000 A characters.
/* Called by one CPU */
void kernelMain(void) {
    using namespace VMM;
    auto ide = StrongPtr<Ide>::make(1, 1);
    auto fs = StrongPtr<Ext2>::make(ide);
    auto root = fs->root;
    auto file1 = fs->find(root,"file1");

    //Test 1: This test case tests that you do lazy loading and have a correct unmap function
   for (int i = 0; i < 100000; i++) {
       auto p = (char*) naive_mmap(file1->size_in_bytes(),false,file1,0);
       CHECK(p == (char*) 0x80000000);
       naive_munmap(p);
   }
    Debug::printf("*** Test 1 cleared\n");

    //Test 2: Shared VME list protected by a lock?
    StrongPtr<Barrier> b1 = StrongPtr<Barrier>::make(101);
     for (int i = 0; i < 100; i++) {
         thread([b1, file1]() {
             char** allocs = new char*[10];
             for (int j = 0; j < 10; j++) {
                 auto p = (char*) naive_mmap(file1->size_in_bytes(),true,file1,0);
                 allocs[j] = p;
             }

             for (int j = 0; j < 10; j++) {
                //won't actually do anything (no-op), but still implicit checks your code 
                //doesn't segfault or do anything weird
                naive_munmap(allocs[j]);
             }
             delete[] allocs;
             b1->sync();
         });
     }
    b1->sync();
    CHECK(privateStart(file1) == (char*) 0x80000000);
    Debug::printf("*** Test 2 cleared\n");
    

    //Test 3: Is your private memory truly private?? Or can other threads access it?
    StrongPtr<Barrier> b2 = StrongPtr<Barrier>::make(101);
     for (int i = 0; i < 100; i++) {
         thread([file1, b2]() {
            auto p = (char*) naive_mmap(file1->size_in_bytes(),false,file1,0);
             CHECK(p[0] == 'A');
             p[0] = 'B';
             
             //increase probability of race conditions
             sleep(1);
             naive_munmap(p);
             b2->sync();
         });
     }
    b2->sync();
    Debug::printf("*** Test 3 cleared\n");
    
    //Test 4: Does your naive_munmap(p) accept arguments that aren't page aligned?
     for (int i = 0; i < 4096; i++) {
        auto p = (char*) naive_mmap(file1->size_in_bytes(),false,file1,0);
        ASSERT(p[0] == 'A');
        p[0] = 'B';
        
        naive_munmap(p + i);
     }
    CHECK(privateStart(file1) == (char*) 0x80000000);
    Debug::printf("*** Test 4 cleared\n"); 

    //Test 5: Does you allow naive_mmap to be called on data pages that haven't been allocated yet 
    //(because you're lazy loading)?
    for (uint32_t i = 0; i < 10000; i++) {
        auto p = (char*) naive_mmap(file1->size_in_bytes(),false,file1,i);
        naive_munmap(p);
    }
    CHECK(privateStart(file1) == (char*) 0x80000000);
    Debug::printf("*** Test 5 cleared\n"); 
}


