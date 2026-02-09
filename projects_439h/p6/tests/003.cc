#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"

/*
This TC stresses many mmap calls without an unmap to test your implementation
for dynamic array/linked list or whatever other data structure (I am the OG stress test). It also checks for 
correct removal and a relative efficient method for insertion/search (maintaining some
form of sorted data structure will likely do best here). I don't test shared mapping, so this
is specifically to stress test many mmap and unmap calls in various scenarios.

This does stress that you try to optimize to better than a linear scanning linked list. You may time
out on this with a linked list, so you should try to get some form of O(log n) search. Overall, this case
is simple yet effective in what it tests.
*/

uint32_t N = 20000;
uint32_t M = 1000;
uint32_t FAULTS_PER_MMAP = 30;

void kernelMain(void) {

    using namespace VMM;
    

    auto ide = StrongPtr<Ide>::make(1, 1);
    
    auto fs = StrongPtr<Ext2>::make(ide);
   
    auto root = fs->root;

   
    auto file = fs->find(root,"file1");
    
    //let's test your basic functionality of mmap first

    auto f = (char*) naive_mmap(file->size_in_bytes(),false,file,0);
    Debug::printf("%s \n", f);
    //let's unmap this now, this should clear everything
    naive_munmap(f);

    f = (char*)naive_mmap(file->size_in_bytes(),false,StrongPtr<Node> {},0);
    Debug::printf("*** what is in our file now: %s \n", f); //we should see nothing here
    naive_munmap(f); 

    Debug::printf("*** End of Part 1 reached! \n");

    Debug::printf("*** Now, let's stress out your mmap implementation a bit \n");

    //If you are using a dynamic array, you are likely going to need to resize here, so make sure this doesn't time out
    //if you are using a linked list, this won't be a super big deal for you
    
    for(uint32_t i = 0; i < N; i++){
        f = (char*)naive_mmap(file->size_in_bytes(),false,file,0);
        uint32_t target_address = 0x80000000 + i * 4096; //we expect to bump allocate here
        ASSERT(f == (char*)target_address);
    }
    
    Debug::printf("*** You allocate stressfully in a straight line! Let's test some unmapping now\n");
    
    //this may take a while if your unmap is slow, you should be able to see the difference

    for(uint32_t i = 0; i < N/200; i++){
        uint32_t unmap_addr = 0x80000000 + i * 4096;
        naive_munmap((char*)unmap_addr);
        //let's remap it so that we can test some things later
        f = (char*)naive_mmap(file->size_in_bytes(),false,file,0);
        ASSERT(f == (char*)unmap_addr);
    }

    Debug::printf("*** Binary search performs better here but a linear scan still passes. Congrats on having a working unmap and mmap and not timing out! \n");

    Debug::printf("*** Let's move on to the next part, where we perform searches in the later half of the array, which will likely be much slower for linear scans \n");

    for(uint32_t i = 199*N/200; i < N; i++){
        uint32_t unmap_addr = 0x80000000 + i * 4096;
        naive_munmap((char*)unmap_addr);
        //let's remap it so that we can test some things later
        f = (char*)naive_mmap(file->size_in_bytes(),false,file,0);
        ASSERT(f == (char*)unmap_addr);
    }

    

    Debug::printf("*** This last part should take roughly the same amount of time as the previous section so you can judge your optimizations based on that.\n");

    //first, let's unmap everything so we have space for our final test

    for(uint32_t i = 0; i < N; i++){
        uint32_t unmap_addr = 0x80000000 + i * 4096;
        naive_munmap((char*) unmap_addr);
    }
    
    Debug::printf("*** Now we just force load memory. This is where binary search really shines!\n");

    char** pointers = new char*[M];
    
    for(uint32_t i = 0; i < M; i++){
        uint32_t target_addr = 0x80000000 + i * 4096 * FAULTS_PER_MMAP;
        auto f = (char*)naive_mmap(4096*FAULTS_PER_MMAP - 1,false,file,0); //fill our array
        ASSERT((uint32_t)f == target_addr);
        pointers[i] = f;
    }

    for(uint32_t i = 0; i < M; i++){
        char* pointer = pointers[i];
        for(uint32_t j = 0; j < FAULTS_PER_MMAP; j++){
            pointer[i + j*4096] = 'a'; //load all the pages of memory
            ASSERT(pointer[i + j*4096] == 'a');
        }
    }

    Debug::printf("*** That's all for my tc!\n");
    
}
