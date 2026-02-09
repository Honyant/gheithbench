#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"

//future taken from t0
template <typename T, typename Work>
inline StrongPtr<Promise<T>> future(Work const& work) {
    auto p = StrongPtr<Promise<T>>::make();
    thread([p, work] {
        p->set(work());
    });
    return p;
}

/*this tests some edge cases in how you determine what virtual address you alloc pages at*/
//make sure you are allocating the earliest free region that fits the size
//make sure munmap deallocates the entire region that includes the address, not just one page
//also a basic test of shared vs private memory
void kernelMain(void) {
    using namespace VMM;
    int testNum = 1;
    //test 1 - basic mmap with size aligned to page_size
    Debug::printf("*** test %d - basic mmap with size aligned to page_size\n",testNum++);
    void* p1 = naive_mmap(0x1000,false,StrongPtr<Node>{},0);
    CHECK(p1 == (void*) 0x80000000);
    Debug::printf("*** p1 = %x\n",p1);
    Debug::printf("*** \n");

    //test 2 - basic mmap with size not aligned to page_size
    Debug::printf("*** test %d - basic mmap with size not aligned to page_size\n",testNum++);
    void* p2 = naive_mmap(0x1313,false,StrongPtr<Node>{},0);
    Debug::printf("*** p2 = %x\n",p2);
    CHECK(p2 == (void*) 0x80001000);
    
    void* p3 = naive_mmap(0x0420,false,StrongPtr<Node>{},0);
    Debug::printf("*** p3 = %x\n",p3);
    CHECK(p3 == (void*) 0x80003000);
    Debug::printf("*** \n");

    //test 3 - basic munmap
    Debug::printf("*** test %d - basic munmap\n",testNum++);
    naive_munmap(p2);
    Debug::printf("*** unmapped p2\n");
    Debug::printf("*** \n");

    //test 4 - make sure earliest free region that fits size is mapped first
    Debug::printf("*** test %d - make sure earliest free region that fits size is mapped first\n", testNum++);
    void* p4 = naive_mmap(0x2345,false,StrongPtr<Node>{},0); 
    //p4 can't fit between p1 and p3 (gap is 2 pages, we need 3 for this), so we put it after p3
    Debug::printf("*** p4 = %x\n",p4);
    CHECK(p4 == (void*) 0x80004000);
    
    void* p5 = naive_mmap(0x0620,false,StrongPtr<Node>{},0);
    //p5 can fit between p1 and p3, only takes up 1 page
    Debug::printf("*** p5 = %x\n",p5);
    CHECK(p5 == (void*) 0x80001000);
    
    void* p6 = naive_mmap(0x0740,false,StrongPtr<Node>{},0);
    //p6 can fit between p5 and p3, takes up 1 page
    CHECK(p6 == (void*) 0x80002000);
    Debug::printf("*** p6 = %x\n",p6);
    Debug::printf("*** \n");

    //test 5 - munmap first entry and then insert back at the start of free memory
    Debug::printf("*** test %d - munmap first entry and then insert back at the start of free memory\n",testNum++);
    naive_munmap(p1);
    Debug::printf("*** unmapped p1\n");
    void* p9 = naive_mmap(0x1000,false,StrongPtr<Node>{},0);
    Debug::printf("*** p9 = %x\n",p9);
    CHECK(p9 == (void*) 0x80000000); //make sure p9 can be entered before the first vme since there's free space
    Debug::printf("*** \n");

    //test 6 - munmap on an address in the middle of a vme
    Debug::printf("*** test %d - munmap on an address in the middle of a vme\n",testNum++);
    naive_munmap((void*) ((uint32_t) p5+0x0130));
    Debug::printf("*** unmapped p5\n");
    void* p7 = naive_mmap(0x1000,false,StrongPtr<Node>{},0);
    Debug::printf("*** p7 = %x\n",p7);
    CHECK(p7 == (void*) 0x80001000); //make sure p5 was freed correctly 
    Debug::printf("*** \n");

    //test 7 - munmap on a region spanning multiple pages -> should dealloc all pages
    Debug::printf("*** test %d - munmap on a region spanning multiple pages\n",testNum++);
    naive_munmap((void*) ((uint32_t) p4+0x1234)); //p4 spans 3 pages, this should dealloc all 3 pages
    Debug::printf("*** unmapped p4\n");
    void* p8 = naive_mmap(0x1000,false,StrongPtr<Node>{},0);
    Debug::printf("*** p8 = %x\n",p8);
    CHECK(p8 == (void*) 0x80004000); //make sure p4 was freed correctly
    Debug::printf("*** \n");
    
    //test 8 - shared vs priv memory
    Debug::printf("*** test %d - shared vs priv memory\n",testNum++);
    void* shared1 = naive_mmap(0x1000,true,StrongPtr<Node>{},0);
    Debug::printf("*** shared1 = %x\n",shared1);
    CHECK(shared1 == (void*) 0xF0000000);
    ((char*)shared1)[0] = 'V';

    auto child1 = future<bool>([shared1] {
        ((char*)shared1)[1] = 'Z';
        CHECK(((char*)shared1)[0] == 'V'); //should see the 'V' written from main thread
        return true;
    });
    child1->get();

    ((char*)p1)[0] = 'S';

    auto child2 = future<bool>([p1] {
        void* priv1 = naive_mmap(0x1000, false, StrongPtr<Node>{}, 0);
        Debug::printf("*** priv1 = %x\n",priv1);
        CHECK(priv1 == (void*) 0x80000000);
        ((char*)priv1)[0] = 'C';
        return true;
    });
    child2->get();

    CHECK(((char*)shared1)[1] == 'Z'); //should see the 'Z' written from child1 thread
    CHECK(((char*)p1)[0] == 'S'); //should see the 'S' written from main thread
    Debug::printf("*** \n");
}

