#include "ext2.h"
#include "ide.h"
#include "kernel.h"
#include "libk.h"


void show(const char *name, StrongPtr<Node> node, bool show);

void kernelMain(void) {
    auto ide = StrongPtr<Ide>::make(1, 3);
    auto fs = StrongPtr<Ext2>::make(ide);

    Debug::printf("*** block size is %d\n", fs->get_block_size());
    Debug::printf("*** inode size is %d\n", fs->get_inode_size());

    auto root = fs->root;
    show("/", root, true);
    
    {
        // Checking ./ and ../ finds
        Debug::printf("*** Testing ./ and ../ finds\n");
        auto result = fs->find(root, ".");
        show("/.", result, true);
        CHECK(root->number == result->number);

        result = fs->find(root, "./.");
        show("/./.", result, true);
        CHECK(root->number == result->number);

        result = fs->find(root, "././.");
        show("/././.", result, true);
        CHECK(root->number == result->number);

        result = fs->find(root, "./././.");
        show("/./././.", result, true);
        CHECK(root->number == result->number);

        result = fs->find(root, "././././.");
        show("/././././.", result, true);
        CHECK(root->number == result->number);

        result = fs->find(root, "./././././.");
        show("/./././././.", result, true);
        CHECK(root->number == result->number);

        result = fs->find(root, "./../.././.././..");
        show("/./../.././.././..", result, true);
        CHECK(root->number == result->number);
        Debug::printf("*** Done with part 1!\n\n");
    }

    {
        // Check file path parsing (specifically paths that are subsets)
        Debug::printf("*** Testing file path parsing (Specifically for paths that are subsets)\n");
        auto hello = fs->find(root, "hello");
        auto helloWorld = fs->find(root, "helloWorld");
        auto helloWorld1 = fs->find(root, "helloWorld1");
        auto hiddenHello = fs->find(root, ".hello");
        auto hiddenHiddenHello = fs->find(root, "..hello");

        CHECK(hello->number != helloWorld->number);
        CHECK(helloWorld->number != helloWorld1->number);
        CHECK(helloWorld1->number != hiddenHello->number);
        CHECK(hiddenHello->number != hiddenHiddenHello->number);
        CHECK(hiddenHello->number != root->number);
        CHECK(hiddenHiddenHello->number != root->number);

        Debug::printf("*** Done with part 2!\n\n");

        // Check finds for directories
        Debug::printf("*** Testing find in directories\n");
        auto data = fs->find(root, "data");
        auto helloRoot = fs->find(data, "/hello");
        auto helloData = fs->find(data, "hello");

        show("/data/hello", helloData, false);
        CHECK(helloRoot->number == hello->number);
        CHECK(helloData->number != hello->number);

        auto result = fs->find(root, "/data/data1/data2/data3/data4/hi");
        show("/data/data1/data2/data3/data4/hi", result, true);

        Debug::printf("*** Done with part 3!\n\n");
    }

    // Testing Symlink Traversal
    // Debug::printf("*** Testing Symlink Traversal\n");
    // auto result = fs->find(root, "link2/");
    // show("/link2", result, true);
    // auto result = fs->find(root, "/data/data1/data2/data3/data4/link/");
    // show("/data/data1/data2/data3/data4/link/", result, true);
    // Debug::printf("*** Test case complete!\n\n");
}

void show(const char *name, StrongPtr<Node> node, bool show) {

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
        CHECK(K::strlen(buffer) == cnt);
        // can't just print the string because there is a 1000 character limit
        // on the output string length.
        for (uint32_t i = 0; i < cnt; i++) {
        Debug::printf("%c", buffer[i]);
        }
        delete[] buffer;
        Debug::printf("\n");
    }
    } else {
    Debug::printf("***    is of type %d\n", node->get_type());
    }
}
