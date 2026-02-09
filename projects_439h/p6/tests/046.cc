#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"



template <typename T, typename Work>
inline StrongPtr<Promise<T>> future(Work const& work) {
    auto p = StrongPtr<Promise<T>>::make();
    thread([p, work] {
        p->set(work());
    });
    return p;
}


/*
    this tc hunts for lurking race conditions and ghostly bugs 
    in your shared memory implementation....
    we will exorcise memory regions of their spooky surprises, 
    ensuring no thread is left haunted by unexpected behavior
    
    happy halloween! 
*/


void kernelMain(void) {

    using namespace VMM;

    //make a shared mem region of 1 page
    auto shared_mem = (char*) naive_mmap(4096, true, StrongPtr<Node>{}, 0);
    
    //check that it exists
    CHECK(shared_mem != nullptr);

    //check that we're at the right place
    CHECK(shared_mem == (char*) 0xF0000000);

    shared_mem[0] = 'B';
    shared_mem[1] = 'o';
    shared_mem[2] = 'o';
    shared_mem[3] = '!';
    shared_mem[4] = '\0';
    Debug::printf("*** scary shared: %s\n", shared_mem);

    //check that child can see it, then modify
    auto child = future<bool>([shared_mem] {
        CHECK(shared_mem[0] == 'B');
        CHECK(shared_mem[3] == '!');
        
        //let's make shared memeory less scary!
        shared_mem[3] = '?';  
        return true;
    });

    child->get();

    //check that one character in mem got changed
    Debug::printf("*** less scary if this worked: %s\n", shared_mem);
    CHECK(shared_mem[3] == '?');

    //now, test two threads writing in "Trick" or "Treat"
    auto writer1 = future<bool>([shared_mem] {
        for (int i = 0; i < 4090; i += 5) {
            shared_mem[i] = 'T';
            shared_mem[i + 1] = 'r';
            shared_mem[i + 2] = 'i';
            shared_mem[i + 3] = 'c';
            shared_mem[i + 4] = 'k';
        }
        return true;
    });

    writer1->get();

    auto writer2 = future<bool>([shared_mem] {
        for (int i = 2040; i < 4090; i += 5) {
            shared_mem[i] = 'T';
            shared_mem[i + 1] = 'r';
            shared_mem[i + 2] = 'e';
            shared_mem[i + 3] = 'a';
            shared_mem[i + 4] = 't';
        }
        return true;
    });

    writer2->get();

    //check trick or treat
    for (int i = 0; i < 2040; i += 5) {
        CHECK(shared_mem[i] == 'T' && shared_mem[i + 1] == 'r' &&
              shared_mem[i + 2] == 'i' && shared_mem[i + 3] == 'c' &&
              shared_mem[i + 4] == 'k');
    }

    for (int i = 2040; i < 4090; i += 5) {
        CHECK(shared_mem[i] == 'T' && shared_mem[i + 1] == 'r' &&
              shared_mem[i + 2] == 'e' && shared_mem[i + 3] == 'a' &&
              shared_mem[i + 4] == 't');
    }

    Debug::printf("*** you got trick or treat mapped!\n");

    
    Debug::printf("*** you finished all the checks, happy halloween!\n");
}