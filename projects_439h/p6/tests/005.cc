#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"
#include "barrier.h"

// largefile.txt is a 16KB + 1 byte file filled with the letter A + null terminator at the end

/* Called by one CPU */
void kernelMain(void) {

    using namespace VMM;

    // --------------------------------------------------------------------
    // edge case testing - mapping at address space boundaries
    // --------------------------------------------------------------------
    Debug::printf("*** edge case testing - mapping at address space boundaries\n");
    // this should map to the edge of shared address space
    uint32_t edgeSize = 4096; // 4KB
    uint32_t edgeAddress = 0xFFFFFFFF - edgeSize + 1; // the last page
    auto edgeMap = (char*) naive_mmap(edgeSize, true, StrongPtr<Node>(), 0);
    ASSERT(edgeMap != nullptr);
    ASSERT((uintptr_t)edgeMap <= edgeAddress);

    // write and read to the edge mapping
    edgeMap[0] = 'E';
    CHECK(edgeMap[0] == 'E');
    Debug::printf("*** edge mapping successful!\n");
    Debug::printf("*** section complete!\n");
}
