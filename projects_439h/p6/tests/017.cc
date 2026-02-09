#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "vmm.h"
#include "debug.h"
#include "physmem.h"
#include "promise.h"
#include "barrier.h"

void check_addr(const char* p_name, char* p, uint32_t addr) {
    if(p != (char*)addr) 
        Debug::printf("*** D: %s is at 0x%x instead of 0x%x\n", p_name, p, addr);
}

void kernelMain(void) {
    using namespace VMM;
    using namespace PhysMem;

    auto ide = StrongPtr<Ide>::make(1, 1);
    auto fs = StrongPtr<Ext2>::make(ide);
    auto root = fs->root;
    
    auto bla = fs->find(root, "bla.txt");

    // check that you do lazy loading
    char* p1 = nullptr;
    for(int i = 0; i < 30000; i++) {
        // mmap with offset not 0 and requiring multiple pages
        p1 = (char *)naive_mmap(bla->size_in_bytes(), false, bla, 297);
        check_addr("p1", p1, 0x80000000);
        if(i != 30000-1) naive_munmap(p1);
    }

    p1[51] = '\0';
    Debug::printf("*** %s\n", p1);

    // page fault in second page of mmapped space
    p1[FRAME_SIZE + 7 + 47] = '\0';
    Debug::printf("*** %s\n", p1 + FRAME_SIZE + 7);

    // testing shared memory
    Promise<bool>* promise1 = new Promise<bool>();
    Barrier* barrier = new Barrier(3);

    auto hello = fs->find(root, "hello.txt");
    char* p2 = (char*)naive_mmap(hello->size_in_bytes() + FRAME_SIZE, true, hello, 0);
    check_addr("p2", p2, 0xF0000000);
    p2[108] = '\0';
    Debug::printf("%s", p2);
    promise1->set(true);

    thread([promise1, barrier] {
        promise1->get();
        char* p3 = (char*)(0xF0000000 + 109);
        Debug::printf("%s\n", p3);
        barrier->sync();
    });

    thread([promise1, barrier] {
        promise1->get();
        char* p5 = (char*)(0xF0000000 + 109);
        Debug::printf("%s\n", p5);

        char* p4 = (char*)naive_mmap(1, true, StrongPtr<Node>{}, 0);
        check_addr("p4", p4, 0xF0002000);
        barrier->sync();
    });

    barrier->sync();
    Debug::printf("*** yay!\n");
}