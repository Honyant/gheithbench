#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"
#include "barrier.h"
#include "random.h"


/* Called by one CPU */
void kernelMain(void) {

    using namespace VMM;


    // IDE device #1
    auto ide = StrongPtr<Ide>::make(1, 1);
    
    // We expect to find an ext2 file system there
    auto fs = StrongPtr<Ext2>::make(ide);
   
   // get "/"
   auto root = fs->root;

   //  get "/treasure.txt" and "/lost_treasure.txt"
   auto treasure = fs->find(root,"treasure.txt");
   auto lost_treasure = fs->find(root, "lost_treasure.txt");

   auto p = (char*) naive_mmap(treasure->size_in_bytes(),false,treasure,0);
   CHECK(p == (char*) 0x80000000);
   Debug::printf("*** Test 1 - Simple mmap\n");
   Debug::printf("%s",p);

   naive_munmap((void*)p); // testing simple unmap

   p = (char*) naive_mmap(treasure->size_in_bytes(),false,treasure,0);
   CHECK(p == (char*) 0x80000000);
   Debug::printf("*** Test 2 - Result of ummap\n");
   Debug::printf("%s",p);

   naive_munmap((void*)(p + 5)); // testing unmap that does not begin in the region

   auto q = (char*) naive_mmap(lost_treasure->size_in_bytes() + 4096,true,lost_treasure,0);
   CHECK(q == (char*) 0xF0000000);
   Debug::printf("*** Test 3 - Shared memory mmap\n");
   Debug::printf("%s",q);


   uint32_t start_threads = 10;
   Barrier* b = new Barrier(start_threads + 1);

   Debug::printf("*** Test 4 - Shared memory access\n");
   for (uint32_t ix = 0; ix < start_threads; ix++) {
    thread([q, b] {
        Debug::printf("*** Printing %s", q);
        b->sync();
    });
   }

   b->sync();

   Debug::printf("*** Test 5 - Efficient mmap/unmap/page faults\n");

   // This essentially maps a lot of files to first increase the length of the virtual memory entry data structure
   // then it samples key points within the data structure and causes page faults continuously. Once a page is 
   // resolved, it will immediately unmap that page, and remap it. This means the implementation must have a fast 
   // overall implementation in order to pass this testcase

   uint32_t mappings = 17500;
   char** pointers_in_mmaps = (char**) malloc(6 * sizeof(char*));

   for (uint32_t i = 0; i < mappings; i++) {
    auto mmp = (char*) naive_mmap(treasure->size_in_bytes(),false,treasure,0);
    if (i == mappings / 4) {
        pointers_in_mmaps[0] = mmp;
    }
    else if (i == mappings / 2) {
        pointers_in_mmaps[2] = mmp;
    }
    else if (i == mappings - 1) {
        pointers_in_mmaps[5] = mmp;
    }
    else if (i == 3 * mappings / 8) {
        pointers_in_mmaps[1] = mmp;
    }
    else if (i == 7 * mappings / 8) {
        pointers_in_mmaps[4] = mmp;
    }
    else if (i == 3 * mappings / 4) {
        pointers_in_mmaps[3] = mmp;
    }
   }

   Debug::printf("*** Finished mapping\n");

   char* answers = (char*)malloc(6 * sizeof(char));
   answers[0] = 't';
   answers[1] = 'r';
   answers[2] = 'e';
   answers[3] = 'a';
   answers[4] = 's';
   answers[5] = 'u';
   for (uint32_t q = 0; q < mappings; q++) {
    uint32_t ix = q % 6;

    ASSERT(pointers_in_mmaps[ix][ix + 11] == answers[ix]);
    naive_munmap(pointers_in_mmaps[ix]);
    naive_mmap(treasure->size_in_bytes(),false,treasure,0);
   }

   Debug::printf("*** Good job!\n");
}

