#include "barrier.h"
#include "ext2.h"
#include "ide.h"
#include "kernel.h"
#include "libk.h"
#include "promise.h"

// This test case tests double indirection (two level indirection)
// It tests zero files (where all the values in the file are zero, and the filesystem optimizes it to take up zero sectors despite having nonzero i_size)
// It stresses intelligent caching
// With intelligent per sector caching, it should only need a roughly 279*512 byte cache to pass = 142kb which is less than half of 300kb, the size of the disk.
// This also tests symlinks with long names, and verifies the symlink name is retrived properly
void show(const char* name, StrongPtr<Node> node, bool show) {
    Debug::printf("*** looking at %s\n", name);
    if (node == nullptr) {
        Debug::printf("***      does not exist\n");
        return;
    }

    if (node->is_dir()) {
        Debug::printf("***      is a directory\n");
        Debug::printf("***      contains %d entries\n", node->entry_count());
        Debug::printf("***      has %d links\n", node->n_links());
    } else if (node->is_symlink()) {
        Debug::printf("***      is a symbolic link\n");
        auto sz = node->size_in_bytes();
        Debug::printf("***      link size is %d\n", sz);
        auto buffer = new char[sz + 1];
        buffer[sz] = 0;
        node->get_symbol(buffer);
        Debug::printf("***       => %s\n", buffer);
    } else if (node->is_file()) {
        Debug::printf("***      is a file\n");
        auto sz = node->size_in_bytes();
        Debug::printf("***      contains %d bytes\n", sz);
        Debug::printf("***      has %d links\n", node->n_links());
        if (show) {
            auto buffer = new char[sz + 1];
            buffer[sz] = 0;
            auto cnt = node->read_all(0, sz, buffer);
            CHECK(sz == cnt);
            // CHECK(K::strlen(buffer) == cnt);
            // can't just print the string because there is a 1000 character
            // limit on the output string length.
            for (uint32_t i = 0; i < cnt; i++) {
                if(buffer[i]==0) break;
                Debug::printf("%c", buffer[i]);
            }
            delete[] buffer;
            Debug::printf("\n");
        }
    } else {
        Debug::printf("***    is of type %d\n", node->get_type());
    }
}

constexpr uint32_t N = 5;

/* Called by one core */
void kernelMain(void) {
    // IDE device #1
    auto ide = StrongPtr<Ide>::make(1, 4);

    // We expect to find an ext2 file system there
    auto fs = StrongPtr<Ext2>::make(ide);

    Debug::printf("*** block size is %d\n", fs->get_block_size());
    Debug::printf("*** inode size is %d\n", fs->get_inode_size());

    // get "/"
    auto root = fs->root;
    show("/", root, true);

    //  get "/hello"
    auto hello = fs->find(root, "hello");
    CHECK(root->number != hello->number);
    show("/hello", hello, true);

    // get "/goodbye"
    auto goodbye = fs->find(root, "goodbye");
    CHECK(hello->number != goodbye->number);
    CHECK(root->number != goodbye->number);
    show("/goodbye", goodbye, true);

    // get "/not_there"
    auto not_there = fs->find(root, "not_there");
    show("/not_there", not_there, true);

    // get "/shortname"
    auto shortname = fs->find(root, "shortname");
    show("/shortname", shortname, true);

    // get "/empty"
    auto empty = fs->find(root, "empty");
    show("/empty", empty, true);

    Debug::printf("*** Zero file test: file that is all zeros. takes up zero sectors. tests that you handle sparse blocks\n");
    // get "/zerofile"
    auto zerofile = fs->find(root, "zerofile");
    show("/zerofile", zerofile, true);

    Debug::printf("*** Double indirection test + mass caching test\n");
    // get "/fortunes_tripled"
    auto fortunes_tripled = fs->find(root, "fortunes_tripled");
    show("/fortunes_tripled", fortunes_tripled, false);
    {
        Debug::printf("***      ");
        for(int j=0;j<60;j++){
            uint32_t start = 0;
            uint32_t count = 279;
            for (uint32_t i = 0; i < count; i++) {
                char c; 
                fortunes_tripled->read(start + i * 1023, c);
                if(j==0) Debug::printf("%c", c);
            }
        }
        Debug::printf("\n");
    }

    // get "/."
    show("/.", fs->find(root, "."), true);
    CHECK(root->number == fs->find(root, ".")->number);

    // get "/.."
    show("/..", fs->find(root, ".."), true);
    CHECK(root->number == fs->find(root, "..")->number);

    // get "/data"
    auto data = fs->find(root, "data");
    show("/data", data, true);
    CHECK(root->number == fs->find(data, "..")->number);
    CHECK(data->number == fs->find(data, ".")->number);
    CHECK(data->number != root->number);

    // get "/data/data.txt"
    auto data_txt = fs->find(data, "data.txt");
    show("/data/data.txt", data_txt, true);
    {
        uint32_t start = 17;
        auto sz = data_txt->size_in_bytes();
        CHECK(sz >= start);
        auto buffer = new char[sz - start + 1];
        for (uint32_t n = 4; n < sz - start; n++) {
            buffer[n] = 0;
            auto cnt = data_txt->read_all(start, n, buffer);
            CHECK(cnt == n);
            Debug::printf("%s\n", buffer);
        }
        delete[] buffer;
    }

    // get "/data/panic.txt"
    show("/data/panic.txt", fs->find(data, "panic.txt"), true);
}