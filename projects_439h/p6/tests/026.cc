#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"
#include "physmem.h"

void kernelMain(void) {
    using namespace VMM;
    using namespace PhysMem;

    ////////////////////////////////////////////////////////
    // This is a simple test to debug your mmap insertion //
    ////////////////////////////////////////////////////////

    // mmap size larger than private memory
    ASSERT(naive_mmap((0xF0000000 - 0x80000000 + 1), false, {}, 0) == nullptr);

    // now let's try again
    // this should fit exactly
    auto p = naive_mmap((0xF0000000 - 0x80000000), false, {}, 0);
    CHECK(p == (char*)0x80000000);

    // unmap it and try something else
    naive_munmap(p);

    // map two halves of shared memory
    auto a = naive_mmap((0xF0000000 - 0x80000000) / 2, false, {}, 0);
    auto b = naive_mmap((0xF0000000 - 0x80000000) / 2, false, {}, 0);
    CHECK(a == (char*)0x80000000);
    CHECK(b == (char*)(0x80000000 + 0x38000000));

    // unmap the first half
    naive_munmap(a);

    // and make sure we can add it back in at the front 
    a = naive_mmap((0xF0000000 - 0x80000000) / 2, false, {}, 0);
    CHECK(a == (char*)0x80000000);

    // let's do quarters now
    naive_munmap(a);
    a = naive_mmap((0xF0000000 - 0x80000000) / 4, false, {}, 0);
    auto c = naive_mmap((0xF0000000 - 0x80000000) / 4, false, {}, 0);
    CHECK(a == (char*)0x80000000);
    CHECK(c == (char*)(0x80000000 + 0x38000000 / 2));

    // unmap the second quarter
    naive_munmap(c);

    // this should not fit
    ASSERT(naive_mmap((0xF0000000 - 0x80000000) / 4 + 1, false, {}, 0) == nullptr);

    // map a little less than a quarter
    a = naive_mmap((0xF0000000 - 0x80000000) / 4 - FRAME_SIZE, false, {}, 0);
    CHECK(a == (char*)(0x80000000 + 0x38000000 / 2));
    // this should fit 
    c = naive_mmap(1, false, {}, 0);
    CHECK(c == (char*)(0x80000000 + 0x38000000 - FRAME_SIZE));

    // we should be full now
    ASSERT(naive_mmap(1, false, {}, 0) == nullptr);

    // last check that mapping size of 0 doesn't crash
    naive_mmap(0, false, {}, 0);

    Debug::printf("*** done!\n");
}
