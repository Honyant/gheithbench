#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"
#include "barrier.h"

void kernelMain(void) {
    using namespace VMM;

    auto ide = StrongPtr<Ide>::make(1, 1);
    auto fs = StrongPtr<Ext2>::make(ide);
    auto root = fs->root;


    // TEST 1: mmap files
    Debug::printf("*** TEST 1\n");

    StrongPtr<Node> file = fs->find(root, "file");
    char *p1 = (char*) naive_mmap(file->size_in_bytes(), false, file, 0);
    Debug::printf("*** p1 address: 0x%x\n", p1);
    Debug::printf("*** p1 data: %s\n", p1);

    StrongPtr<Node> directory = fs->find(root, "directory");
    StrongPtr<Node> nested_file = fs->find(directory, "nested_file");
    char *p2 = (char*) naive_mmap(nested_file->size_in_bytes(), false, nested_file, 0);
    Debug::printf("*** p2 address: 0x%x\n", p2);
    Debug::printf("*** p2 data: %s\n", p2);

    Debug::printf("***\n");


    // TEST 2: mmap with offset
    Debug::printf("*** TEST 2\n");

    char *p3 = (char*) naive_mmap(file->size_in_bytes() - 5, false, file, 5);
    Debug::printf("*** p3 address: 0x%x\n", p3);
    Debug::printf("*** p3 data: %s\n", p3);

    char *p4 = (char*) naive_mmap(1, false, file, 12);
    Debug::printf("*** p4 address: 0x%x\n", p4);
    Debug::printf("*** p4 data: %c\n", p4[0]);
    Debug::printf("*** p4 + 2 data: %d\n", p4[2]);

    Debug::printf("***\n");


    // TEST 3: munmap files
    Debug::printf("*** TEST 3\n");

    naive_munmap(p4);
    char *p5 = (char*) naive_mmap(1, false, StrongPtr<Node>{}, 0);
    Debug::printf("*** p5 address: 0x%x\n", p5);

    naive_munmap(p2);
    char *p6 = (char*) naive_mmap(1, false, StrongPtr<Node>{}, 0);
    Debug::printf("*** p6 address: 0x%x\n", p6);
    Debug::printf("*** p6 data: %d\n", p6[0]);

    Debug::printf("***\n");


    // TEST 4: Separate threads with private mapping
    Debug::printf("*** TEST 4\n");
    int *p[5];

    for (int i = 0; i < 3; i++) {
        Barrier *b = new Barrier(2);

        thread([&p, i, b] {
            p[i] = (int*) naive_mmap(1, false, StrongPtr<Node>{}, 0);
            Debug::printf("*** p%d address: 0x%x\n", i, p[i]);
            Debug::printf("*** p%d data: %d\n", i, p[i][0]);
            p[i][0] = i;
            b->sync();
        });

        b->sync();
    }

    for (int i = 0; i < 3; i++) {
        Barrier *b = new Barrier(2);

        thread([&p, i, b] {
            p[i] = (int*) naive_mmap(1, false, StrongPtr<Node>{}, 0);
            Debug::printf("*** p%d address: 0x%x\n", i, p[i]);
            Debug::printf("*** p%d data: %d\n", i, p[i][0]);
            p[i][0] = i;
            b->sync();
            naive_munmap(p[i]);
        });

        b->sync();
    }

    Debug::printf("***\n");

    
    // TEST 5: Separate threads with shared mapping
    Debug::printf("*** TEST 5\n");

    Barrier *b1 = new Barrier(2);
    Barrier *b2 = new Barrier(2);
    Barrier *b3 = new Barrier(2);

    char *p7;

    thread([&p7, b1, b2, b3, file] {
        p7 = (char*) naive_mmap(file->size_in_bytes(), true, file, 0);
        Debug::printf("*** p7 address: 0x%x\n", p7);
        Debug::printf("*** p7 data: %s\n", p7);
        b1->sync();
        b2->sync();
        Debug::printf("*** p7 altered data: %s\n", p7);
        b3->sync();
    });

    b1->sync();

    p7[6] = 'h';
    p7[7] = 'a';
    p7[8] = 'n';
    p7[9] = 'g';
    p7[10] = 'e';
    p7[11] = 'd';
    p7[12] = '!';

    b2->sync();
    b3->sync();
}
