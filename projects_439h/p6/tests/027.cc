#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"
#include "barrier.h"	

const uint32_t THREADS_TC = 50;
const uint32_t PRIVATE_VA_START_TC = 0x80000000;
const uint32_t SHARED_VA_START_TC = 0xF0000000;
const uint32_t VA_END_TC = 0xFFFFFFFF;
const uint32_t PAGE_SIZE_TC = 4096;

/* Called by one CPU */
void kernelMain(void) {

    using namespace VMM;

    auto ide = StrongPtr<Ide>::make(1, 1);
    auto fs = StrongPtr<Ext2>::make(ide);
    auto root = fs->root;

    Debug::printf("*** Test 1: Memory and Demand Paging\n");

    /*
        1. fill up entirety of private VA space
        2. should not be allocated or will cause OOM
    */
    for (uint32_t i = 0; i < THREADS_TC; i++) {
        thread([i, root] {
            auto p = (uint32_t*)naive_mmap(SHARED_VA_START_TC - PRIVATE_VA_START_TC, false, StrongPtr<Node> {}, 0);
            ASSERT(p[0] == 0);  
            p[0] = i;
            ASSERT(p[0] == i);
        });
    }

    auto p = (uint32_t)naive_mmap(SHARED_VA_START_TC - PRIVATE_VA_START_TC - PAGE_SIZE_TC, false, StrongPtr<Node> {}, 0);
    ASSERT(p == 0x80000000);

    // handle edge case of allocating past boundary (could result in overflow !!)
    auto not_enough_space = naive_mmap(SHARED_VA_START_TC - PRIVATE_VA_START_TC, false, StrongPtr<Node> {}, 0);
    ASSERT(not_enough_space == nullptr);

    naive_munmap((void*)p);


    p = (uint32_t)naive_mmap(SHARED_VA_START_TC - PRIVATE_VA_START_TC, false, StrongPtr<Node> {}, 0);
    ASSERT(p == 0x80000000);
    naive_mmap(0, false, StrongPtr<Node> {}, 0);

    naive_munmap((void*)p);

    Debug::printf("*** Test 1 Passed\n");


    Debug::printf("*** Test 2: Shared Memory\n");
    uint32_t size = THREADS_TC;
    auto shared = (uint32_t)naive_mmap(size, true, StrongPtr<Node> {}, 0);
    StrongPtr<Barrier> barrier2 = StrongPtr<Barrier>::make(THREADS_TC + 1);

    for (uint32_t i = 0; i < THREADS_TC; i++) {
        thread([i, shared, barrier2] {
            auto p = (uint32_t*)shared;
            p[i] = i;
            barrier2->sync();
        });
    }

    barrier2->sync();
    auto shared_arr = (uint32_t *)shared;

    Debug::printf("*** ");
    for (uint32_t i = 0; i < THREADS_TC; i++) {
        Debug::printf("%d ", shared_arr[i]);
    }
    Debug::printf("\n*** Test 2 Passed\n");


    /*
    This is just for fun !! We can parallel compute for some LC-style problems
    so that each thread does O(1) work and we can get the result in O(1) time (sometimes :D)
    */
    Debug::printf("*** Test 3: Applications\n");
    auto input = fs->find(root, "input.txt");
    auto n = input->size_in_bytes();
    auto arr = (char*)naive_mmap(n, true, input, 0);
    StrongPtr<Barrier> barrier = StrongPtr<Barrier>::make(n / 2 + 1);

    Debug::printf("*** Starting array: ");
    for (uint8_t i = 0; i < n; i++) {
        Debug::printf("%c ", arr[i]);
    }
    Debug::printf("\n");

    // REVERSE ARR IN PARALLEL
    for (uint8_t i = 0; i < n / 2; i++) {
        thread([i, n, arr, barrier] {
            char temp = arr[i];
            arr[i] = arr[n - i - 1];
            arr[n - i - 1] = temp;
            barrier->sync();
        });
    }

    barrier->sync();

    Debug::printf("*** Reversed array: ");
    for (uint8_t i = 0; i < n; i++) {
        Debug::printf("%c ", arr[i]);
    }
    Debug::printf("\n*** Test 3 Passed\n");
}
