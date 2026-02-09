#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"
#include "barrier.h"

template <typename T, typename Work>
inline StrongPtr<Promise<T>> future(Work const& work) {
    auto p = StrongPtr<Promise<T>>::make();
    thread([p, work] {
        p->set(work());
    });
    return p;
}

/* Called by one CPU */
void kernelMain(void) {
    using namespace VMM;
    auto ide = StrongPtr<Ide>::make(1, 1);
    auto fs = StrongPtr<Ext2>::make(ide);
    Barrier *b1 = new Barrier(21);
    Barrier *b2 = new Barrier(21);

    auto root = fs->root;
    auto node = fs->find(root,"onepage");
    auto bignode = fs->find(root,"bigfile.txt");

    /* Test 1: Many small allocs and deallocs */
    Debug::printf("*** Test 1: ");
    uint32_t test1_loops = 100;
    for (uint32_t i = 0; i < test1_loops; i++) {
        auto p = (char*) naive_mmap(node->size_in_bytes(),false,node,0);
        ASSERT(p == (char*) 0x80000000);
        naive_munmap((void *) p);
    }
    Debug::printf("passed\n");

    /* Test 2: One large alloc, deref, and dealloc */
    Debug::printf("*** Test 2: ");
    char* q = (char*) naive_mmap(bignode->size_in_bytes(),false,bignode,0);
    ASSERT(q == (char*) 0x80000000);
    Debug::printf("%c", q[0]);
    Debug::printf("%c", q[1]);
    naive_munmap((void *) q);
    Debug::printf("ssed\n");

    /* Test 3: Large shared alloc and dealloc (no deref) across threads */
    Debug::printf("*** Test 3: ");
    uint32_t test3_loops = 20;
    for (uint32_t i = 0; i < test3_loops; i++) {
        thread([b1, bignode] {
            auto p = (char*) naive_mmap(bignode->size_in_bytes(),false,bignode,0);
            naive_munmap((void *) p);
            b1->sync();
        });
    }
    b1->sync();
    Debug::printf("passed\n");

    /* Test 4: Small shared alloc and deref across threads */
    Debug::printf("*** Test 4: ");
    uint32_t test4_loops = 20;
    char* r = (char*) naive_mmap(node->size_in_bytes(),true,node,0);
    for (uint32_t i = 0; i < test4_loops; i++) {
        thread([r, b2] {
            ASSERT(r[0] == 'H');
            ASSERT(r[1] == 'e');
            ASSERT(r[2] == 'y');
            b2->sync();
        });
    }
    b2->sync();
    Debug::printf("passed\n");

    /* Test 5: Unmap Shifted VA */
    Debug::printf("*** Test 5: ");
    char* va = (char*) naive_mmap(node->size_in_bytes(),false,node,0);
    uint32_t val = (uint32_t) va;
    va = va + 4095;
    naive_munmap((void *) va);
    char* va2 = (char*) naive_mmap(node->size_in_bytes(),false,node,0);
    naive_munmap((void *) va2);
    ASSERT(val == ((uint32_t) va2));
    Debug::printf("passed\n");
}

