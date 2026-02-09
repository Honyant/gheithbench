#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "barrier.h"
#include "vmm.h"
#include "debug.h"

struct DeletionBarrier {
    StrongPtr<Barrier> b;
    DeletionBarrier(StrongPtr<Barrier> b) : b(b) {}
    ~DeletionBarrier() {
        b->sync();
    }
};

/* Called by one CPU */
void kernelMain(void) {

    // Even getting kernelMain to run is a big accomplishment! Be proud of yourself!
    Debug::printf("*** You made it to the start of the test case! Nice!\n");

    using namespace VMM;

    // Load filesystem
    auto ide = StrongPtr<Ide>::make(1, 1);
    auto fs = StrongPtr<Ext2>::make(ide);
    auto root = fs->root;

    //  get "/hello"
    auto hello = fs->find(root,"hello.txt");
    auto len = hello->size_in_bytes();


    Debug::printf("*** -------- Test 1 --------\n");

    // Do a simple mmap!
    char* buf = (char*) naive_mmap(len, false, hello, 0);
    Debug::printf("*** We did a map: 0x%x -> hello.txt\n",buf);
    Debug::printf("%s",buf);


    Debug::printf("*** -------- Test 2 --------\n");

    // The APIC reserves a frame at 0xfec00000
    // Map the shared space up to that point
    naive_mmap(0xec00000, true, StrongPtr<Node>{}, 0);

    // Map more shared space, which should fall AFTER the APIC's frame (at 0xfec01000)
    char* buf2 = (char*) naive_mmap(len, true, hello, 0);
    Debug::printf("*** We did a shared map: 0x%x -> hello.txt\n",buf2);
    Debug::printf("%s",buf2);

    // Modify shared memory inside of a thread to make sure it affects the main thread.
    buf2[4] = 'E';
    buf2[5] = 'p';
    StrongPtr<Barrier> b {new Barrier(2)};
    thread([b, buf2] {
        buf2[6] = 'i';
        buf2[7] = 'c';
        b->sync();
    });
    b->sync();
    Debug::printf("%s",buf2);

    Debug::printf("*** -------- Test 3 --------\n");

    // Do a simple munmap!
    naive_munmap(buf);
    Debug::printf("*** We unmapped hello.txt from 0x%x\n",buf);

    //  get "/thoughts"
    auto thoughts = fs->find(root,"thoughts.txt");
    auto len2 = thoughts->size_in_bytes();


    Debug::printf("*** -------- Test 4 --------\n");

    // Map thoughts.txt (with an offset to test that) to make sure your munmap
    // actually freed up the space at 0x80000000
    char* buf3 = (char*) naive_mmap(len2, false, thoughts, 4);
    Debug::printf("*** We did another map: 0x%x -> thoughts.txt\n",buf3);
    Debug::printf("%s",buf3);

    // munmap again
    naive_munmap(buf3);


    Debug::printf("*** -------- Test 5 --------\n");

    // Make a file-less map that spans multiple pages
    char* buf4 = (char*) naive_mmap(10000, false, StrongPtr<Node>{}, 0);
    Debug::printf("*** We did yet another map: 0x%x -> [blank space]\n", buf4);
    buf4[4094 + 0] = 'h';  // Try
    buf4[4094 + 1] = 'e';  // writing
    buf4[4094 + 2] = 'l';  // <- (page boundary)
    buf4[4094 + 3] = 'l';  // across
    buf4[4094 + 4] = 'o';  // page
    buf4[4094 + 5] = '\n'; // boundaries
    Debug::printf("*** %s", buf4 + 4094); // Memory should be zero-filled so we don't need a null char at the end

    // munmap again
    naive_munmap(buf4);


    // This tests several complex things... comment it out if you wanna test the simpler stuff above
    // TLDR; we make a bunch of threads that map a bunch of memory, making sure you free it all on thread deletion

    // We run a loop 20 times. Each iteration we create a thread that mmaps all of virtual memory (0x80000000 - 0xf0000000),
    // then writes to about ~1/5 of our physical memory worth of pages (forcing you to allocate them). When the thread
    // is deleted, its DeletionBarrier is deleted, syncing the barrier. This makes sure that the thread actually gets deleted
    // before the loop continues (technically it might not be done yet but since we're only using 1/5 of our memory we
    // shouldn't run out before it gets fully deleted).

    // Things this tests:
    // - You can map all of (private) virtual memory
    // - You don't immediately allocate physical memory (but can when necessary)
    // - When threads are deleted you unmap & free all of their mappings (also free any other data structures!)
    // - Your mmaps are decently efficient
    // - Your munmaps are pretty efficient
    //     - hint: if you try to unmap a page that has no page table, you can skip the other pages in that table's range

    Debug::printf("*** ------------------------\n");
    Debug::printf("*** -------- Test 6 --------\n");
    Debug::printf("*** ------------------------\n");
    Debug::printf("*** Prepare for some lag...\n");
    const uint32_t memAmount = (kConfig.memSize - 0x00600000) / 5; // 1/5 of available physical memory
    for (int i = 1; i <= 20; i++) {
        bool print = i <= 5 || i % 5 == 0;
        if (print) Debug::printf("*** Starting cycle %d\n", i);

        StrongPtr<Barrier> b {new Barrier(2)};
        thread([=] {
            DeletionBarrier db(b);
            if (print) Debug::printf("*** Mapping all virtual memory...\n");

            char* p = (char*) naive_mmap(0x70000000, false, StrongPtr<Node>{}, 0); // mmap all of private virtual memory
            ASSERT(p);
            for (uint32_t i = 0; i < memAmount; i += 0x1000) { // force 1/5 of physical memory to get allocated
                p[i] = 'a';
            }

            if (print) Debug::printf("*** You should in theory unmap it now...\n");
            // when the thread's deleted, you should free all the physical memory
        });

        // Wait for the thread to run & get deleted
        b->sync();
        if (print) Debug::printf("*** Cycle %d complete!\n", i);
    }
}
