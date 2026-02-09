#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"
#include "atomic.h"
#include "barrier.h"

Atomic<uint32_t> step{1};

// this test does not have shared mem, since there are already a bunch of tests for that

/* Called by one CPU */
void kernelMain(void) {

    using namespace VMM;

    // IDE device #1
    auto ide = StrongPtr<Ide>::make(1, 1);
    
    // We expect to find an ext2 file system there
    auto fs = StrongPtr<Ext2>::make(ide);
   
   // get "/"
   auto root = fs->root;

    // PART 1
    // offset test
    for (uint32_t i = 1; i < 4; i++) {
        auto character = fs->find(root, "data/welcome.txt");
        auto welcome_text = (char*) naive_mmap(28,false,character,28 * i);
        for (int i = 0; i < 28; i++) {
            Debug::printf("%c", welcome_text[i]);
        }

        naive_munmap(welcome_text);
    }

    // PART 2
    // test that you don't crash when size = 0
    naive_mmap(0, false, StrongPtr<Node>{}, 0);

    // PART 3
    // test for accessing in the middle of an mmapped area
    auto charList = fs->find(root, "data/character_list");
    auto va = (char*) naive_mmap(82,false,charList,0);
    Debug::printf("*** %s\n", va + 65);

    // PART 4
   // multithreading test
    auto b = new Barrier(6);
    for (uint32_t i = 1; i <= 5; i++) {
        thread([i, root, &fs, b] {
            char* char_str = new char[24];
            ::memcpy(char_str, "sanrio_world/character", 22); 
            char_str[22] = '0' + i;
            char_str[23] = 0;
            auto character = fs->find(root, char_str);
            auto text = (char*) naive_mmap(character->size_in_bytes(),false,character,0);
            while (step.get() != i) ;
            Debug::printf("*** %s\n", text);
            step.fetch_add(1);
            b->sync();
        });
    }
    b->sync();
}
