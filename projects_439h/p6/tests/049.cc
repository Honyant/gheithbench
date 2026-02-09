#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "random.h"

/* Called by one CPU */
void kernelMain(void) {

    using namespace VMM;

    // Test 1: Basic mmap/munmap

    // IDE device #1, no disk read delay
    auto ide = StrongPtr<Ide>::make(1, 0);
    // We expect to find an ext2 file system there
    auto fs = StrongPtr<Ext2>::make(ide);
   
   // get "/"
   auto root = fs->root;

   //  get "/hello"
   auto hello = fs->find(root,"hello");
   
   // hello is a single page
   char* p = (char*) naive_mmap(hello->size_in_bytes(), false, hello, 0);
   // first page
   CHECK(p == (char*) 0x80000000);
   
   // unmap
   naive_munmap(p);
   
   // remap
   p = (char*) naive_mmap(hello->size_in_bytes(), false, hello, 0);
   // first page
   CHECK(p == (char*) 0x80000000);
   naive_munmap(p);
   
   Debug::printf("*** Basic mmap/munmap successful\n");
   
   // Test 2: mmap at scale (still linear)
   // Checks that you use some efficient data structure to manage your pages
   
   for (int i = 0; i < 10000; i++) {
        auto p = (char*) naive_mmap(hello->size_in_bytes(), false, hello, 0);
        p[0] = 'a';
        // new page for each iteration (sequential)
        CHECK(p == (char*) (0x80000000 + i * 4096));
   }
   
   Debug::printf("*** Mmap at scale successful (linear)\n");
   
   // test random, non-sequential unmapping
   Random rng(0);
   bool* unmapped = new bool[10000]; // track which pages have been unmapped
   int unmapped_count = 0;
   while (unmapped_count < 5000) {
        uint32_t random_page_idx = rng.next() % 10000;
        if (!unmapped[random_page_idx]) {
            naive_munmap((void*) (0x80000000 + random_page_idx * 4096));
            unmapped[random_page_idx] = true;
            unmapped_count++;
        }
   }
   
   Debug::printf("*** Random, non-sequential unmapping successful\n");
   
   for (int i = 0; i < 10000; i++) {
        if (unmapped[i]) {
            auto p = (char*) naive_mmap(hello->size_in_bytes(), false, hello, 0);
            p[0] = 'a';
        }
   }
   
   Debug::printf("*** Random, non-sequential remapping successful (good paging data structure!)\n");
   
}
