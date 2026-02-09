#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"
#include "barrier.h"
#include "debug.h"

// This attempts to be a comprehensive unmap test case

/* Called by one CPU */
void kernelMain(void) {
    using namespace VMM;
    
    // IDE device #1
    auto ide = StrongPtr<Ide>::make(1, 1);
    
    // We expect to find an ext2 file system there
    auto fs = StrongPtr<Ext2>::make(ide);
    
    // get "/"
    auto root = fs->root;
    
    // get test files
    auto small_file = fs->find(root,"tate.txt");
    auto large_file = fs->find(root, "tatemcrae.txt");
    
    Debug::printf("*** Starting munmap tests\n");
    
    // Test 1: Basic unmap functionality
    Debug::printf("*** Test 1: Basic unmap\n");
    auto p1 = (char*) naive_mmap(small_file->size_in_bytes(), false, small_file, 0);
    CHECK(p1 == (char*) 0x80000000);
    Debug::printf("*** 1.1: Initial mapping successful\n");
    
    naive_munmap(p1);
    Debug::printf("*** 1.2: Unmap completed\n");
    
    // Verify by mapping again to same address
    auto p2 = (char*) naive_mmap(small_file->size_in_bytes(), false, small_file, 0);
    CHECK(p2 == (char*) 0x80000000);
    Debug::printf("*** 1.3: Remap successful\n");
    naive_munmap(p2);
    
    // Test 2: Multiple sequential mappings and unmaps
    Debug::printf("*** Test 2: Sequential mappings\n");
    char* ptrs[5];
    for (int i = 0; i < 5; i++) {
        ptrs[i] = (char*) naive_mmap(4096, false, small_file, 0);
        CHECK(ptrs[i] != nullptr);
        Debug::printf("*** 2.%d: Mapped to %x\n", i+1, (uint32_t)ptrs[i]);
    }
    
    // Unmap in order
    for (int i = 0; i < 5; i++) {
        naive_munmap(ptrs[i]);
        Debug::printf("*** 2.%d: Unmapped from %x\n", i+6, (uint32_t)ptrs[i]);
    }
    
    // Test 3: Unmap middle of region
    Debug::printf("*** Test 3: Unmap middle of region\n");
    auto p3 = (char*) naive_mmap(4096 * 10, false, small_file, 0); // 10 pages
    Debug::printf("*** 3.1: Large region mapped at %x\n", (uint32_t)p3);
    
    // Write some data
    for (int i = 0; i < 10; i++) {
        p3[i * 4096] = 'A' + i;
    }
    
    // Unmap from middle
    naive_munmap(p3 + 4096 * 5);
    Debug::printf("*** 3.2: Middle unmap completed\n");
    
    // Map new region where middle was unmapped
    auto p3_middle = (char*) naive_mmap(4096, false, small_file, 0);
    Debug::printf("*** 3.3: New middle mapping at %x\n", (uint32_t)p3_middle);
    naive_munmap(p3);

    // Should have fixed double unmap
    // naive_munmap(p3_middle);
    
    // Test 4: Unmap non-aligned address
    Debug::printf("*** Test 4: Unmap non-aligned address\n");
    auto p4 = (char*) naive_mmap(4096 * 2, false, small_file, 0);
    Debug::printf("*** 4.1: Mapped at %x\n", (uint32_t)p4);
    naive_munmap(p4 + 100); // Non-page-aligned
    Debug::printf("*** 4.2: Non-aligned unmap completed\n");
    
    // Test 5: Shared memory unmap
    Debug::printf("*** Test 5: Shared memory unmap\n");
    auto shared1 = (char*) naive_mmap(4096, true, small_file, 0);
    auto shared2 = (char*) naive_mmap(4096, true, small_file, 0);
    CHECK(shared1 == (char*)0xF0000000);
    CHECK(shared2 == (char*)0xF0001000);
    Debug::printf("*** 5.1: Shared regions mapped at %x and %x\n", 
                 (uint32_t)shared1, (uint32_t)shared2);
    
    naive_munmap(shared1);
    Debug::printf("*** 5.2: First shared region unmapped\n");
    naive_munmap(shared2);
    Debug::printf("*** 5.3: Second shared region unmapped\n");
    
    // Test 6: Performance - large region handling
    Debug::printf("*** Test 6: Large region handling\n");
    const int LARGE_PAGES = 1000; // 4MB total
    auto large_ptr = (char*) naive_mmap(4096 * LARGE_PAGES, false, large_file, 0);
    CHECK(large_ptr != nullptr);
    Debug::printf("*** 6.1: Large region mapped at %x\n", (uint32_t)large_ptr);
    
    // Access some pages to force faults
    for (int i = 0; i < LARGE_PAGES; i += 50) {
        large_ptr[i * 4096] = 'X';
    }
    Debug::printf("*** 6.2: Pages accessed\n");
    
    naive_munmap(large_ptr);
    Debug::printf("*** 6.3: Large region unmapped\n");
    
    // Test 7: Alternating map/unmap
    Debug::printf("*** Test 7: Alternating map/unmap\n");
    const int NUM_ITERATIONS = 100;
    char* last_ptr = nullptr;
    
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        auto ptr = (char*) naive_mmap(4096 * ((i % 10) + 1), false, small_file, 0);
        CHECK(ptr != nullptr);
        if (last_ptr) {
            naive_munmap(last_ptr);
        }
        last_ptr = ptr;
        
        if (i % 10 == 0) {
            Debug::printf("*** 7.%d: Iteration complete\n", i/10);
        }
    }
    if (last_ptr) naive_munmap(last_ptr);
    
    Debug::printf("*** All munmap tests passed!\n");
}