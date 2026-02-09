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
 
 
void kernelMain(void) {
    auto ide = StrongPtr<Ide>::make(1, 3);
    auto fs = StrongPtr<Ext2>::make(ide);

    Debug::printf("*** File System Block Size: %d\n", fs->get_block_size());
    Debug::printf("*** Inode Size: %d\n", fs->get_inode_size());

    auto root = fs->root;
    show("/", root, true);

    auto simple_file = fs->find(root, "simple/simple_file.txt");
    show("/simple/simple_file.txt", simple_file, true);

    auto non_existent_file = fs->find(root, "does_not_exist.txt");
    show("/does_not_exist.txt", non_existent_file, true);

    auto complex_symlink = fs->find(root, "complex_symlink/");
    show("/complex_symlink/", complex_symlink, true);

    auto large_dir = fs->find(root, "large_dir");
    show("/large_dir", large_dir, false);

    for (char c = 'a'; c <= 'z'; c++) {
        char name[] = {c, 0};
        auto node = fs->find(large_dir, name);
        if (node!=nullptr && node->is_file() && node->size_in_bytes() == 0) {
            Debug::printf("*** File %s is correctly empty.\n", name);
        } else {
            Debug::printf("*** Problem in file %s\n", name);
            show(name, node, true);
        }
    }

    for (int i =0; i< 1; i++){
        non_existent_file = fs->find(root, "large_dir/does_not_exist.txt");
    }

    auto relative_symlink = fs->find(root, "relative_link/");
    show("/relative_link/", relative_symlink, true);

    auto nested_dir = fs->find(root, "nested/link_to_file/");
    show("/nested/link_to_file/", nested_dir, true);

}