#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"

void kernelMain(void)
{
    using namespace VMM;

    auto ide = StrongPtr<Ide>::make(1, 1);

    auto fs = StrongPtr<Ext2>::make(ide);
    auto root = fs->root;

    auto large_file = fs->find(root, "large_file");
    auto file_size = large_file->size_in_bytes();

    auto p = (char *)naive_mmap(file_size, false, large_file, 0);
    CHECK(p == (char *)0x80000000);
    Debug::printf("*** Mapped large file\n");

    // Stress test: Map the memory many times but do not dereference
    for (int i = 0; i < 15000; ++i)
    {
        auto temp_map = naive_mmap(4096, false, StrongPtr<Node>{}, 0);
        CHECK(temp_map != nullptr);
    }

    Debug::printf("*** Stress test completed without loading memory\n");

    Debug::printf("*** Dereferencing to trigger lazy load\n");
    char value = p[0];

    CHECK(value == 'L');
    Debug::printf("*** Lazy load successful, first byte: %c\n", value);

    auto another_chunk = naive_mmap(4096, false, large_file, 4096);
    CHECK(another_chunk != nullptr);
    char *another_chunk_casted = (char *)another_chunk;
    char another_value = another_chunk_casted[0];
    Debug::printf("*** Lazy load of another chunk: %c\n", another_value);

    Debug::printf("*** Passed lazy load test\n");
}
