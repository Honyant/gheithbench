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


    // IDE device #1
    auto ide = StrongPtr<Ide>::make(1, 1);
    
    // We expect to find an ext2 file system there
    auto fs = StrongPtr<Ext2>::make(ide);
   
   // get "/"
   auto root = fs->root;
   const char** names = new const char*[4];
   names[0] = "hello0";
   names[1] = "hello1";
   names[2] = "hello2";
   names[3] = "hello3";

    // check vm is thread private
   Barrier* b = new Barrier(5);
    for(int i = 0; i < 4; i++){
        thread([names, fs, root, i, b]{
            sleep(i);
            auto p = (uint32_t*) naive_mmap(1,false,StrongPtr<Node>{},0);
            CHECK(p == (uint32_t*) 0x80000000);
            CHECK(p[0] == 0);
            CHECK(p[1] == 0);
            CHECK(p[2] == 0);
            auto hello = fs->find(root,names[i]);

            auto q = (char*) naive_mmap(hello->size_in_bytes() + 4096,false,hello,0);
            CHECK(q == (char*) 0x80001000);
            Debug::printf("*** %d\n",i);
            Debug::printf("%s",q);
            b->sync();
        });
    }
   b->sync();
}

