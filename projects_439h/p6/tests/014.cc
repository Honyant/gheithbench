#include "barrier.h"
#include "config.h"
#include "ext2.h"
#include "ide.h"
#include "libk.h"
#include "promise.h"
#include "shared.h"
#include "vmm.h"

uint32_t file_name_len = 4;

/* Called by one CPU */
void kernelMain(void) {
    using namespace VMM;

    auto ide = StrongPtr<Ide>::make(1, 1);
    auto fs = StrongPtr<Ext2>::make(ide);
    auto root = fs->root;

    // TEST 1: Basic Shared Memory Test: Simply makes sure that modifications to
    // a page in VM modify the same physical page
    auto start = fs->find(root, "start.txt");

    char* p = (char*)naive_mmap(start->size_in_bytes(), true, start, 0);
    CHECK(p == (char*)0xF0000000);
    auto name = fs->find(root, "filename.txt");
    char* good = (char*)naive_mmap(name->size_in_bytes(), true, name, 0);

    Barrier* b1 = new Barrier(file_name_len + 1);
    for (uint32_t i = 0; i < file_name_len; i++) {
        thread([i, p, good, b1] {
            p[i] = good[i];
            b1->sync();
        });
    }
    b1->sync();

    char* file_name = (char*)malloc((file_name_len + 5) * sizeof(char));
    file_name[4] = '.';
    file_name[5] = 't';
    file_name[6] = 'x';
    file_name[7] = 't';
    file_name[8] = '\0';

   
    for (uint32_t i = 0; i < file_name_len; i++) {
        file_name[i] = p[i];
    }

    auto file1 = fs->find(root, file_name);
    auto t1 = naive_mmap(file1->size_in_bytes(), false, file1, 0);
    Debug::printf("*** %s\n", t1);
    naive_munmap(t1);

    // TEST 2: Ensures that private mem is actually private; also makes sure
    // that you use demand paging
    auto smth = fs->find(root, "smth.txt");
    auto q = naive_mmap(smth->size_in_bytes(), false, smth, 0);

    Barrier* b2 = new Barrier(51);
    for (uint32_t i = 0; i < 50; i++) {
        // If you don't use demand paging, you will run out of memory
        thread([b2] {
            auto q = (char*)naive_mmap(0xF0000000 - 0x80000000, false,
                                       StrongPtr<Node>{}, 0);
            CHECK((uint32_t)q == 0x80000000);
            q[0] = 'L';
            naive_munmap(q);
            b2->sync();
        });
    }
    b2->sync();

    Debug::printf("*** %s\n", q);
    naive_munmap(q);

    // TEST 3: Situations where mmap should return null

    auto null1 = (char*)naive_mmap(0xF0000000 - 0x80000000 + 1, false,
                                   StrongPtr<Node>{}, 0);
    CHECK(null1 == nullptr);

    auto r =
        (char*)naive_mmap(0xF0000000 - 0x80000000, false, StrongPtr<Node>{}, 0);
    auto null2 = (char*)naive_mmap(1, false, StrongPtr<Node>{}, 0);
    CHECK((uint32_t)r == 0x80000000);
    CHECK(null2 == nullptr);
    naive_munmap(r);

    auto null3 =
        (char*)naive_mmap(0xFFFFFFFF - 0xF0000000, true, StrongPtr<Node>{}, 0);
    CHECK(null3 == nullptr);

    Debug::printf("*** Goppert Bagel with Jalapeños\n");
}
