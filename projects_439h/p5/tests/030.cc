#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"
#include "threads.h"
#include "barrier.h"

void show(const char* name, StrongPtr<Node> node, bool show) {
    Debug::printf("*** looking at %s\n",name);

    if (node == nullptr) {
        Debug::printf("***      does not exist\n");
        return;
    } 

    if (node->is_dir()) {
        Debug::printf("***      is a directory\n");
        Debug::printf("***      contains %d entries\n",node->entry_count());
        Debug::printf("***      has %d links\n",node->n_links());
    } else if (node->is_symlink()) {
        Debug::printf("***      is a symbolic link\n");
        auto sz = node->size_in_bytes();
        Debug::printf("***      link size is %d\n",sz);
        auto buffer = new char[sz+1];
        buffer[sz] = 0;
        node->get_symbol(buffer);
        Debug::printf("***       => %s\n",buffer);
    } else if (node->is_file()) {
        Debug::printf("***      is a file\n");
        auto sz = node->size_in_bytes();
        Debug::printf("***      contains %d bytes\n",sz);
        Debug::printf("***      has %d links\n",node->n_links());
        if (show) {
            auto buffer = new char[sz+1];
            buffer[sz] = 0;
            auto cnt = node->read_all(0,sz,buffer);
            CHECK(sz == cnt);
            CHECK(K::strlen(buffer) == cnt);
            // can't just print the string because there is a 1000 character limit
            // on the output string length.
            for (uint32_t i=0; i<cnt; i++) {
                Debug::printf("%c",buffer[i]);
            }
            delete[] buffer;
            Debug::printf("\n");
        }
    } else {
        Debug::printf("***    is of type %d\n",node->get_type());
    }
}

void kernelMain(void) {
    auto ide = StrongPtr<Ide>::make(1, 3);
    auto fs = StrongPtr<Ext2>::make(ide);

    Debug::printf("*** block size is %d\n",fs->get_block_size());
    Debug::printf("*** inode size is %d\n",fs->get_inode_size());
   
    auto root = fs->root;
    show("/", root, true);

    uint32_t N = 20;

    /*
        The goal of this test is to make multiple threads iterate through
        directories at the same time to test for race conditions.
    */

    // TEST 1
    {
        auto b = new Barrier(N + 1);
        uint32_t entry_counts[N];

        for (uint32_t i = 0; i < N; i++) {
            thread([i, &b, &root, &entry_counts]{
                entry_counts[i] = root->entry_count();
                b->sync();
            });
        }

        b->sync();

        const uint32_t expected_entry_count = 6;
        for (uint32_t i = 0; i < N; i++) {
            ASSERT(entry_counts[i] == expected_entry_count);
        }

        Debug::printf("*** entry counts match expected\n");
    }

    // TEST 2
    {
        auto b = new Barrier(N + 1);
        StrongPtr<Node> not_there[N];

        for (uint32_t i = 0; i < N; i++) {
            thread([i, &b, &root, &not_there, &fs]{
                not_there[i] = fs->find(root,"not_there");
                b->sync();
            });
        }

        b->sync();

        for (uint32_t i = 0; i < N; i++) {
            ASSERT(not_there[i] == nullptr);
        }

        Debug::printf("*** find test 1 matches expected\n");
    }

    // TEST 3
    {
        auto b = new Barrier(N + 1);
        char** there = new char*[N];

        for (uint32_t i = 0; i < N; i++) {
            thread([i, &b, &root, &there, &fs] {
                there[i] = new char[1];
                auto c = fs->find(root,"c");
                if (i == 0) show("/c", c, true);
                c->read_all(0, 1, there[i]);
                b->sync();
            });
        }

        b->sync();

        char* expected_text = new char[1];
        expected_text[0] = 'c';
        for (uint32_t i = 0; i < N; i++) {
            ASSERT(there[i][0] == expected_text[0]);
        }

        Debug::printf("*** find test 2 matches expected\n");
    }
}

