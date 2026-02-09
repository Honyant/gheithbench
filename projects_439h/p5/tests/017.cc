#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"

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

char* read_to_buffer(StrongPtr<Node> node) {
    auto sz = node->size_in_bytes();
    char* buffer = new char[sz+1];
    node->read_all(0, sz, buffer);
    buffer[sz] = '\0';
    return buffer;
}

/* Called by one core */
void kernelMain(void) {
    auto ide = StrongPtr<Ide>::make(1, 10);
    auto fs = StrongPtr<Ext2>::make(ide);
    auto root = fs->root;

    // basic symlink
    auto dir = fs->find(root, "directory");
    auto first_link = fs->find(root, "directory/link1");
    show("/directory/link1", first_link, true);
    
    char first_link_val [first_link->size_in_bytes()+1];
    first_link_val[first_link->size_in_bytes()] = '\0';
    first_link->get_symbol(first_link_val);
    auto first = fs->find(dir, first_link_val);
    show("/directory/subdirectory/first.txt", first, false);

    // chain of symlinks with rel paths in different directories
    auto second = fs->find(root, "link3/second.txt");
    // also tests that you have a cache (~10KB should be sufficient)
    for(int i = 0; i < 100; i++) second = fs->find(root, "link3/second.txt");
    show("/directory/subdirectory/subsubdirectory/second.txt", second, false);

    auto third = fs->find(dir, "/third.txt");
    show("/third.txt", third, false);

    char* buffer1 = read_to_buffer(first);
    char* buffer2 = read_to_buffer(second);
    char* buffer3 = read_to_buffer(third);
    Debug::printf("%s", buffer1);
    Debug::printf("%s", buffer2);
    Debug::printf("%s", buffer3);
}

