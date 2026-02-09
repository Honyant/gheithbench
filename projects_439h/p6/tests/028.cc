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
        
        Debug::printf("*** hihi\n");
    
    // get "/"
    auto root = fs->root;

    Debug::printf("*** TEST 1: basic mmap\n");

    //  get "/hello"
    auto cat = fs->find(root,"cat");

    auto p = (char*) naive_mmap(cat->size_in_bytes(), false, cat,0);
    Debug::printf("%s\n", p);

    naive_munmap(p);

    Debug::printf("*** TEST 2: basic shared mmap\n");

    auto dog = fs->find(root,"dog");

    p = (char*) naive_mmap(dog->size_in_bytes(), true, dog,0);
    Debug::printf("%s\n", p);

    naive_munmap(p);

    Debug::printf("*** TEST 3: shared mmap\n");

    auto duck = fs->find(root,"duck");

    p = (char*) naive_mmap(duck->size_in_bytes(), true, duck,0);

    Barrier* b = new Barrier(6);

    for (int i = 0; i < 5; i++) {
        thread([p, b]{
            Debug::printf("%s\n", p);
            b->sync();
        });
    }
    b->sync();

    naive_munmap(p);

    Debug::printf("*** TEST 4: unshared mmap\n");
    
    Barrier* b1 = new Barrier(3);
    Barrier* b2 = new Barrier(3);
    Barrier* b3 = new Barrier(3);
    Barrier* b4 = new Barrier(4);

    thread([b1, b2, b3, b4, fs, root] {
        auto duck = fs->find(root,"duck");

        auto p = (char*) naive_mmap(duck->size_in_bytes(), false, duck,0);
        b1->sync();
        b2->sync();
        b3->sync();
        Debug::printf("%s\n", p);
        b4->sync();

    });

    thread([b1, b2, b3, b4, fs, root] {
        auto dog = fs->find(root,"dog");

        auto p = (char*) naive_mmap(dog->size_in_bytes(), false, dog,0);
        b1->sync();
        b2->sync();
        Debug::printf("%s\n", p);
        b3->sync();
        b4->sync();

    });

    thread([b1, b2, b3, b4, fs, root] {
        auto cat = fs->find(root,"cat");

        auto p = (char*) naive_mmap(cat->size_in_bytes(), false, cat,0);
        b1->sync();
        Debug::printf("%s\n", p);
        b2->sync();
        b3->sync();
        b4->sync();
    });

    b4->sync();

    Debug::printf("*** ALL DONE!!\n");

}
