#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"

/*
 * This is a simple test cast that checks basic functionality 
 * 
 * It is intentionally easy to understand and debug to help users make sure their implementation is correct
 */

// this show code is slightly modified from t0
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

// called by one core
void kernelMain(void) {
    // setup
    auto ide = StrongPtr<Ide>::make(1, 3); 
    auto fs = StrongPtr<Ext2>::make(ide); 
    Debug::printf("*** block size is %d\n",fs->get_block_size());
    Debug::printf("*** inode size is %d\n",fs->get_inode_size());
    Debug::printf("*** files have been successfully created\n");
    
    auto root = fs->root; 
    Debug::printf("*** about to look at root\n");
    show("/", fs->root, true);
    Debug::printf("*** successfully looked at root\n");

    // check a few files 
    auto under_root = fs->find(root, "under_root.txt");
    CHECK(root->number != under_root->number); 
    show("/under_root.txt", under_root, true); 

    auto under_root2 = fs->find(root, "under_root2.txt");
    CHECK(root->number != under_root2->number); 
    show("/under_root2.txt", under_root2, true); 

    auto under_root3 = fs->find(root, "under_root3.txt");
    CHECK(root->number != under_root3->number); 
    show("/under_root3.txt", under_root3, true); 

    auto subfile = fs->find(root, "/subdir/under_subdir.txt");
    CHECK(root->number != subfile->number);
    show("/subdir/under_subdir.txt", subfile, true);

    auto subfile2 = fs->find(root, "/subdir/under_subdir2.txt");
    CHECK(root->number != subfile2->number);
    show("/subdir/under_subdir2.txt", subfile2, true);  
}