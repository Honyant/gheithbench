#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"
#include "barrier.h"

/* Called by one CPU */
void kernelMain(void) {
    using namespace VMM;

    // IDE device #1
    auto ide = StrongPtr<Ide>::make(1, 1);
    // We expect to find an ext2 file system there
    auto fs = StrongPtr<Ext2>::make(ide);
    
    // get "/"
    auto root = fs->root;
    
    //  get a file
    auto example_file = fs->find(root,"example_file.txt");

    // test if we can print the contents of the file
    auto p = (char*) naive_mmap(example_file->size_in_bytes(),false,example_file,0);
    for (uint32_t i = 0; i < example_file->size_in_bytes(); i++) {
        Debug::printf("%c", p[i]);
    }
    naive_munmap(p);

    // now we will test effcient usage of shared memory

    // lets define some constants
    uint32_t USER_MEM_START = 0x80000000;
    uint32_t SHARED_MEM_START = 0xF0000000;
    uint32_t RESERVED_SHARED_START = kConfig.ioAPIC;
    uint32_t SHARED_MEM_START_2 = kConfig.ioAPIC + PhysMem::FRAME_SIZE;
    uint32_t RESERVED_SHARED_START_2 = kConfig.localAPIC;
    uint32_t SHARED_MEM_START_3 = kConfig.localAPIC + PhysMem::FRAME_SIZE;
    uint64_t MEMORY_END = 0x100000000;

    // define the sizes of the segments we want to test
    uint32_t PRIVATE_MEM_SIZE = SHARED_MEM_START - USER_MEM_START;
    uint32_t SHARED_MEM_SIZE_1 = RESERVED_SHARED_START - SHARED_MEM_START;
    uint32_t SHARED_MEM_SIZE_2 = RESERVED_SHARED_START_2 - SHARED_MEM_START_2;
    uint32_t SHARED_MEM_SIZE_3 = (uint32_t) (MEMORY_END - (uint64_t)SHARED_MEM_START_3);

    Debug::printf("%x %x %x %x\n",PRIVATE_MEM_SIZE ,SHARED_MEM_SIZE_1 ,SHARED_MEM_SIZE_2, SHARED_MEM_SIZE_3);

    // allocate all of the private memory in multiple chunks
    for (uint32_t mmap_index = 0; mmap_index < 8; mmap_index++) {
        p = (char*) naive_mmap(PRIVATE_MEM_SIZE / 8,false,StrongPtr<Node>{},0);
        ASSERT(p == (char*)(USER_MEM_START + (PRIVATE_MEM_SIZE / 8) * mmap_index));
    }
    // set part of the text in the 8th chunk of the private memory
    p[0] = '*';
    p[1] = '*';
    p[2] = '*';
    p[3] = ' ';
    p[4] = 't';
    p[5] = 'e';
    p[6] = 'x';
    p[7] = 't';
    p[8] = '\n';

    // can you read the text?
    Debug::printf("%s", (char*)(USER_MEM_START + (PRIVATE_MEM_SIZE / 8) * 7));

    // now lets unmap part of the private memory in a weird order to uncover bugs
    naive_munmap((uint32_t*)(USER_MEM_START + (PRIVATE_MEM_SIZE / 8) * 6));
    naive_munmap((uint32_t*)(USER_MEM_START + (PRIVATE_MEM_SIZE / 8) * 7));
    naive_munmap((uint32_t*)(USER_MEM_START + (PRIVATE_MEM_SIZE / 8) * 5));

    // lets now remap those parts of memory
    for (uint32_t mmap_index = 5; mmap_index < 8; mmap_index++) {
        p = (char*) naive_mmap(PRIVATE_MEM_SIZE / 8,false,StrongPtr<Node>{},0);
        ASSERT(p == (char*)(USER_MEM_START + (PRIVATE_MEM_SIZE / 8) * mmap_index));
    }
    // set part of the text in the 8th chunk of the private memory (note that the old text should be cleared out!)
    p[0] = '*';
    p[1] = '*';
    p[2] = '*';
    p[3] = ' ';
    p[4] = 'h';
    p[5] = 'i';
    p[6] = '\n';

    // have you cleared out the text from beforehand?
    Debug::printf("%s", (char*)(USER_MEM_START + (PRIVATE_MEM_SIZE / 8) * 7));

    // now lets unmap part of the private memory in a weird order to uncover bugs again
    naive_munmap((uint32_t*)(USER_MEM_START + (PRIVATE_MEM_SIZE / 8) * 4));
    naive_munmap((uint32_t*)(USER_MEM_START + (PRIVATE_MEM_SIZE / 8) * 5));
    naive_munmap((uint32_t*)(USER_MEM_START + (PRIVATE_MEM_SIZE / 8) * 7));

    // when any of the sizes are not the sizes of pages, they are rounded up anyways

    // NOTE THAT THERE IS GOING TO BE ONE CHUNK MISSING AT THE END (we cut it up into 128 parts but only 127 mmaps)
    for (uint32_t mmap_index = 0; mmap_index < 127; mmap_index++) {
        p = (char*) naive_mmap(SHARED_MEM_SIZE_1 / 128,true,StrongPtr<Node>{},0);
        ASSERT(p == (char*)(SHARED_MEM_START + (SHARED_MEM_SIZE_1 / 128) * mmap_index));
    }

    // even though there is some space at the end of the shared memory, it should put this next one in segment 2
    p = (char*)naive_mmap(SHARED_MEM_SIZE_2,true,StrongPtr<Node>{},0);
    ASSERT(p == (char*)(SHARED_MEM_START_2));

    // the chunk size is small enough to fit in the empty space in shared mem 1
    p = (char*)naive_mmap(PhysMem::frameup(SHARED_MEM_SIZE_3 / 16),true,StrongPtr<Node>{},0);
    ASSERT(p == (char*)(SHARED_MEM_START + (SHARED_MEM_SIZE_1 / 128) * 127));

    // fill up the third segment of shared memory
    p = (char*)naive_mmap(SHARED_MEM_SIZE_3,true,StrongPtr<Node>{},0);
    Debug::printf("SHARED_MEM_SIZE_3 %x\n",SHARED_MEM_SIZE_3);
    Debug::printf("HERE %x\n",p);
    Debug::printf("HERE %x\n",SHARED_MEM_START_3);

    ASSERT(p == (char*)(SHARED_MEM_START_3));

    // fill up the rest of shared memory
    uint32_t cnt = 0;
    while (p != nullptr) {
        p = (char*)naive_mmap(1,true,StrongPtr<Node>{},0);
        if (p != nullptr) {
            ASSERT(p == (char*)(SHARED_MEM_START + (SHARED_MEM_SIZE_1 / 128) * 127) + PhysMem::frameup(SHARED_MEM_SIZE_3 / 16) + (PhysMem::FRAME_SIZE * cnt));
        }
        cnt++;
    }

    // we are out of shared memory
    ASSERT(p == nullptr);

    // DONT OVERFLOW!!
    p = (char*)naive_mmap(1,true,StrongPtr<Node>{},0);
    ASSERT(p == nullptr);

    // DONT OVERFLOW!!
    p = (char*)naive_mmap(2 * PhysMem::FRAME_SIZE,true,StrongPtr<Node>{},0);
    ASSERT(p == nullptr);

    StrongPtr<Barrier> barrier{ new Barrier(11) };

    // all of shared mem is allocated
    char* shared_mem_3 = (char*)SHARED_MEM_START;

    // now lets test some things regarding syncronization
    shared_mem_3[0] = '*';
    shared_mem_3[1] = '*';
    shared_mem_3[2] = '*';
    shared_mem_3[3] = ' ';
    shared_mem_3[4] = 'y';
    shared_mem_3[5] = 'a';
    shared_mem_3[6] = 'y';
    shared_mem_3[7] = ' ';
    shared_mem_3[8] = 'f';
    shared_mem_3[9] = 'o';
    shared_mem_3[10] = 'r';
    shared_mem_3[11] = ' ';
    for (int i = 0; i < 10; i++) {
        thread([SHARED_MEM_START, barrier] {
            char* shared_mem_3 = (char*)SHARED_MEM_START;
            // we set the rest of shared memory inside a different thread than the main thread
            shared_mem_3[12] = 'd';
            shared_mem_3[13] = 'e';
            shared_mem_3[14] = 'b';
            shared_mem_3[15] = 'u';
            shared_mem_3[16] = 'g';
            shared_mem_3[17] = ' ';
            shared_mem_3[18] = 'l';
            shared_mem_3[19] = 'o';
            shared_mem_3[20] = 'c';
            shared_mem_3[21] = 'k';
            shared_mem_3[22] = '!';
            shared_mem_3[23] = '\n';

            Debug::printf("%s", shared_mem_3);

            barrier->sync();
        });
    }

    barrier->sync();
}
