#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"
#include "barrier.h"

uint32_t NUM_LOOPS = 200;
uint32_t NUM_THREADS = 20;

/*
This test is a stress test on your mmap/unmmap implementation, it doesn't test shared memory. The goal of this test is to ensure
that your mmap/unmmap private memory implementation is correct, and you handle edge cases correctly and delete frames correctly. This should
be one of the first test cases you pass as you implement your implementation, and you should debug this before moving on to shared memory!
*/

/* Called by one CPU */
void kernelMain(void) {
     using namespace VMM;

    // IDE device #1
    auto ide = StrongPtr<Ide>::make(1, 1);
    
    // We expect to find an ext2 file system there
    auto fs = StrongPtr<Ext2>::make(ide);
   
    // get "/"
    auto root = fs->root;

    Debug::printf("*** TEST 1: Testing Large File mmap/unmmap\n");

    auto big_file = fs->find(root,"data/bigfile.txt");
    auto p = (char*) naive_mmap(big_file->size_in_bytes(),false,big_file,0);
    CHECK(p == (char*) 0x80000000);
    naive_munmap((void *) p);

    Debug::printf("*** Passed test 1!\n");



    Debug::printf("*** TEST 2: Testing many small file mmap/unmmap\n");

    auto small_file = fs->find(root,"data/smallfile.txt");

    for(uint32_t i = 0; i < NUM_LOOPS; i++) {
        auto p = (char*) naive_mmap(small_file->size_in_bytes(),false,small_file,0);
        CHECK(p == (char*) 0x80000000);
        CHECK(p[0] == 'O');
        CHECK(p[1] == 'K');
        naive_munmap((void *) p);
    }

    Debug::printf("*** Passed test 2!\n");

    Debug::printf("*** TEST 3: Testing null file mmap/unmmap\n");

    auto q = (char*) naive_mmap(32,false,StrongPtr<Node>{},0);
    CHECK(q == (char*) 0x80000000);
    for(uint32_t i = 0; i < 32; i++) {
        CHECK(q[i] == 0);
    }
    naive_munmap((void *) q);

    Debug::printf("*** Passed test 3!\n");

    Debug::printf("*** TEST 4: Testing private memory, multithreaded test\n");

    auto b = new Barrier(NUM_THREADS + 1);
    for(uint32_t i = 0; i < NUM_THREADS; i++) {
        thread([root, &fs, b] {
            auto char_file = fs->find(root, "data/alphabet.txt");
            auto p = (char*)naive_mmap(char_file->size_in_bytes(),false,char_file,0);
            CHECK(p == (char*) 0x80000000);
            for(int j = 0; j < 26; j++) {
                CHECK(p[j] == 'A' + j);
            }
            naive_munmap((void *) p);
            b->sync();
        });
    }
    b->sync();

    Debug::printf("*** Passed test 4!\n");


}