#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "ext2.h"

#define CHUNK_SIZE 0x08000000u

/* Called by one CPU */
void kernelMain(void) {
    using namespace VMM;

    // Let's test your virtual memory fragmentation!
    // Local Virtual Memory extends from 0x80000000 to 0xF0000000 giving us
    // if we divide into 0x08000000 byte chunks, we get 14 chunks.
    // If we treat . as an empty chunk, and # as a filled chunk, our memory should look like this:
    // ..............
    for (int i = 0; i < 14; i++) {
        auto address = naive_mmap(CHUNK_SIZE, false, StrongPtr<Node>{}, 0);
        Debug::printf("Address: %x\n", address);
        Debug::printf("Expected: %x\n", 0x80000000 + i * CHUNK_SIZE);
        ASSERT(address == (char*) (0x80000000 + i * CHUNK_SIZE));
    }

    // Our virtual memory should now look like this:
    // ##############
    //  Attempting another mmap should fail
    ASSERT(nullptr == naive_mmap(CHUNK_SIZE, false, StrongPtr<Node>{}, 0));

    for (int i = 0; i < 14; i += 2) {
        naive_munmap((char*) (0x80000000 + i * CHUNK_SIZE));
    }

    // After freeing every other chunk, our memory should look like this:
    // .#.#.#.#.#.#.#
    
    // This MMAP should fail
    ASSERT(nullptr == naive_mmap(CHUNK_SIZE * 2, false, StrongPtr<Node>{}, 0));
    // This one should succeed
    ASSERT(0x80000000 == (uint32_t)naive_mmap(CHUNK_SIZE, false, StrongPtr<Node>{}, 0));

    // Now our memory should look like this:
    // ##.#.#.#.#.#.#

    // Let's free up some more space
    naive_munmap((char*) 0x80000000 + CHUNK_SIZE * 3);

    // Now our memory should look like this:
    // ##...#.#.#.#.#

    // This MMAP succeed now
    ASSERT(0x80000000 + CHUNK_SIZE * 2 == (uint32_t)naive_mmap(0x10000000, false, StrongPtr<Node>{}, 0));

    // Now our memory should look like this:
    // ##[##].#.#.#.#.#
    // The chunks of memory that were freed should be merged into one mmap

    // Let's free up some more space
    naive_munmap((char*) 0x80000000 + CHUNK_SIZE * 5);
    naive_munmap((char*) 0x80000000 + CHUNK_SIZE * 2);
    
    // Now our memory should look like this:
    // ##.....#.#.#.#

    // This MMAP should fail
    ASSERT(nullptr == naive_mmap(CHUNK_SIZE * 6, false, StrongPtr<Node>{}, 0));
    // But this one should succeed
    ASSERT(0x80000000 + CHUNK_SIZE * 2 == (uint32_t)naive_mmap(CHUNK_SIZE * 5, false, StrongPtr<Node>{}, 0));

    // Now our memory should look like this:
    // ##[#####]#.#.#.#

    // Let's have another go around of it, just to be safe

    naive_munmap((char*) 0x80000000 + CHUNK_SIZE * 1);
    for (int i = 7; i <= 13; i += 2) {
        naive_munmap((char*) 0x80000000 + CHUNK_SIZE * i);
    }

    ASSERT(0x80000000 + CHUNK_SIZE == (uint32_t)naive_mmap(CHUNK_SIZE, false, StrongPtr<Node>{}, 0));
    ASSERT(0x80000000 + CHUNK_SIZE * 7 == (uint32_t)naive_mmap(CHUNK_SIZE * 7, false, StrongPtr<Node>{}, 0));


    // Now our memory should look like this:
    // ##[#####][#######]

    // Now let's free everything

    naive_munmap((char*) 0x80000000);
    naive_munmap((char*) 0x80000000 + CHUNK_SIZE);
    naive_munmap((char*) 0x80000000 + CHUNK_SIZE * 2);
    naive_munmap((char*) 0x80000000 + CHUNK_SIZE * 7);


    // ..............

    // And allocating this monstrosity should succeed

    ASSERT(0x80000000 == (uint32_t)naive_mmap(CHUNK_SIZE * 14, false, StrongPtr<Node>{}, 0));

    Debug::printf("*** Congrats! You made it! ***\n");
}
