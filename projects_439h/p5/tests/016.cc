#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"

void show(const char* name, StrongPtr<Node> node) {
    Debug::printf("*** looking at %s\n", name);

    if (node == nullptr) {
        Debug::printf("***      does not exist\n");
    } 
    else if (node->is_dir()) {
        Debug::printf("***      is a directory\n");
        Debug::printf("***      contains %d entries\n", node->entry_count());
        Debug::printf("***      has %d links\n", node->n_links());
    } 
    else if (node->is_symlink()) {
        Debug::printf("***      is a symbolic link\n");
        auto sz = node->size_in_bytes();
        Debug::printf("***      link size is %d\n", sz);
        auto buffer = new char[sz+1];
        buffer[sz] = 0;
        node->get_symbol(buffer);
        Debug::printf("***       => %s\n", buffer);
    } 
    else if (node->is_file()) {
        Debug::printf("***      is a file\n");
        auto sz = node->size_in_bytes();
        Debug::printf("***      contains %d bytes\n", sz);
        Debug::printf("***      has %d links\n", node->n_links());
        
        auto buffer = new char[sz+1];
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
    else {
        Debug::printf("***    is of type %d\n", node->get_type());
    }
}

void kernelMain(void) {
    StrongPtr<Ide> ide = StrongPtr<Ide>::make(1, 5);
    StrongPtr<Ext2> fs = StrongPtr<Ext2>::make(ide);

    // TEST 1: Root, files, and directories
    Debug::printf("*** TEST 1\n");
    StrongPtr<Node> root = fs->root;
    show("/", root);
    show("/file", fs->find(root, "file"));
    show("/directory", fs->find(root, "directory"));
    show("/directory/", fs->find(root, "directory/"));
    show("/nonexistent", fs->find(root, "nonexistent"));
    show("/directory/nested_file", fs->find(root, "directory/nested_file"));
    show("/directory/nonexistent", fs->find(root, "directory/nonexistent"));
    show("/directory/nested_file", fs->find(fs->find(root, "directory"), "nested_file"));
    Debug::printf("***\n");

    // TEST 2: Current and parent directories
    Debug::printf("*** TEST 2\n");
    show("/.", fs->find(root, "."));
    show("/./", fs->find(root, "./"));
    show("/./.", fs->find(root, "./."));
    show("/directory/..", fs->find(root, "directory/.."));
    show("/directory/../file", fs->find(root, "directory/../file"));
    show("/directory/./../file", fs->find(root, "directory/./../file"));
    show("/directory/../directory/nested_file", fs->find(root, "directory/../directory/nested_file"));
    Debug::printf("***\n");
    
    // TEST 3: Absolute paths
    Debug::printf("*** TEST 3\n");
    StrongPtr<Node> directory = fs->find(root, "directory");
    show("/", fs->find(directory, "/"));
    show("/file", fs->find(directory, "/file"));
    show("/directory", fs->find(directory, "/directory"));
    show("/directory/nested_file", fs->find(directory, "/directory/nested_file"));
    Debug::printf("***\n");

    // TEST 4: Symlinks
    Debug::printf("*** TEST 4\n");
    show("/file_link", fs->find(root, "file_link"));
    show("/directory_link", fs->find(root, "directory_link"));
    show("/directory_link/", fs->find(root, "directory_link/"));
    show("/nested_file_link", fs->find(root, "nested_file_link"));
    show("/directory/root_link", fs->find(root, "directory/root_link"));
    show("/directory_link/nested_file", fs->find(root, "directory_link/nested_file"));
    show("/directory_link/root_link/file", fs->find(root, "directory_link/root_link/file"));
    show("/directory_link/root_link/directory_link/nested_file", fs->find(root, "directory_link/root_link/directory_link/nested_file"));
    Debug::printf("***\n");

    // TEST 5: Potpourri
    Debug::printf("*** TEST 5\n");
    show("/file_link", fs->find(directory, "/file_link"));
    show("/directory_link", fs->find(directory, "/directory_link"));
    show("/directory_link/", fs->find(directory, "/directory_link/"));
    show("/directory/root_link/./", fs->find(directory, "/directory/root_link/./"));
    show("/directory_link/../directory_link/root_link/file", fs->find(root, "directory_link/../directory_link/root_link/file"));
    show("/directory_link/root_link/directory_link/../../../file", fs->find(root, "directory_link/root_link/directory_link/../../../file"));
}
