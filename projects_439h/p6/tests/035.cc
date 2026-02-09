#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"
#include "barrier.h"

/*
    This test has three subtests.
        1. Basic functionality -- Ensures your code can map and unmap to/from correct virtual addresses.
        2. Demand loading -- Maps something the size of usable physical memory, then maps more. 
                             Ensures you can access items using demand loading and without running out of memory. 
        3. Shared memory -- Creates many threads which write to and read from shared memory. 
*/

void kernelMain(void) {
    using namespace VMM;
    auto ide = StrongPtr<Ide>::make(1, 1);
    auto fs = StrongPtr<Ext2>::make(ide);
    auto root = fs->root;

    //  get files
    auto austen_short = fs->find(root,"austen_short.txt");
    auto small_file = fs->find(root, "small_file.txt");

    // TEST 0: Basic Functionality 

    // In each thread, I'll map something, ensure it's mapped correctly, then unmap it and repeat
    int N = 100;
    Barrier* b0 = new Barrier(N+1);
    for (int i = 0; i < N; i++){
        thread([small_file, b0] {
            auto a = (char*) naive_mmap(small_file->size_in_bytes(), false, small_file, 0);
            CHECK(a[0] == 'h');
            CHECK(a == (char*) 0x80000000);
            naive_munmap(a);

            auto b = (char*) naive_mmap(1, false, small_file, 0);
            CHECK(b[0] == 'h');
            CHECK(b == (char*) 0x80000000);
            naive_munmap(b);
            b0->sync();
        });
    }
    b0->sync();
    Debug::printf("*** I hope you are having a good day!\n");

    // TEST 1: Demand Loading
    // First I'll map something the size of usable physical memory
    auto p = (char*) naive_mmap(kConfig.memSize-0x00600000, false, austen_short, 0);

    // now we need to be able to map more things since p never gets demand loaded
    auto q = (char*) naive_mmap(small_file->size_in_bytes(), false, small_file, 0);

    // ensure this memory can be used despite mapping of p
    CHECK(q[0] == 'h');
    CHECK(q[1] == 'e');
    CHECK(q[2] == 'l');
    CHECK(q[3] == 'l');
    CHECK(q[4] == 'o');

    // now, I want to check that we can still demand load p itself (should only load one frame/page), so its ok
    CHECK(p[0] == 'S');
    CHECK(p[1] == 'i');
    CHECK(p[2] == 'r');
    Debug::printf("*** Operating Systems is an interesting class!\n");


    // TEST 2: Shared Memory

    // I will create N threads. Each one will mmap one byte to shared memory, loading the first 
    // N characters of austen_short to the first N bytes of shared memory. 
    // Then each thread (after syncing) will ensure that shared memory is the same as expected from their locally 
    // mapped copy of austen_short
    char** arr = new char*[N];
    Barrier* b = new Barrier(N);
    Barrier* done = new Barrier(N+1);
    Atomic<int>* tracker = new Atomic<int>(0);
    for (int i = 0; i < N; i++){
        thread ([i, arr, austen_short, b, N, done, tracker] {
            while(tracker->get() != i) ; // ensure that threads mmap in order
            auto p = (char*) naive_mmap(N, false, austen_short, 0); // create a local copy of austen_short
            arr[i] = (char*) naive_mmap(1, true, austen_short, i); // contribute to the shared copy of austen_short
            tracker->add_fetch(1);
            b->sync();
            for (int j = 0; j < N; j++){ // ensure local copy and shared copies are identical
                CHECK(p[j] == *arr[j]);
            }
            done->sync();
        });
        
    }

    done->sync();

    Debug::printf("*** Hopefully this test case helps you and your implementation!\n");
}

