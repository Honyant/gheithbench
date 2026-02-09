#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"
#include "barrier.h"


/*
 * My code tests that
 *  - your filename can be 255 bytes long (upper limit as a directory entry only has 1 byte
 *    for the length of a filename)
 *  - you can handle a symlink that references itself (self-referential symlink)
 *  - you can handle a symlink cycle like a -> b -> c where a, b, and c are all symlinks
 */
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

/* Called by one core */
void kernelMain(void) {
    // IDE device #1
    auto ide = StrongPtr<Ide>::make(1, 3);
    
    // We expect to find an ext2 file system there
    auto fs = StrongPtr<Ext2>::make(ide);

    Debug::printf("*** block size is %d\n",fs->get_block_size());
    Debug::printf("*** inode size is %d\n",fs->get_inode_size());
   

   //TEST 1: Max filename length
   auto root = fs->root;
   show("/",root,true);

   // Get the max length filename
    char max_filename[256];
    for (int i = 0; i < 255; i++) {
        max_filename[i] = 'a';
    }
    max_filename[255] = '\0';

    Debug::printf("*** attempting to find the max length filename\n");
    auto max_file = fs->find(root, max_filename);
    show("/[max_filename]", max_file, true);
    Debug::printf("*** Done with Test 1!\n");

    //TEST 2: Get "self_symlink"
    Debug::printf("*** attempting to find 'self_symlink'\n");
    auto self_symlink = fs->find(root, "self_symlink");
    show("/self_symlink", self_symlink, true);

    // self-referential symlink
    if (self_symlink != nullptr && self_symlink->is_symlink()) {
        auto buffer_size = self_symlink->size_in_bytes();
        char* buffer = new char[buffer_size + 1];
        buffer[buffer_size] = '\0';
        self_symlink->get_symbol(buffer);
        Debug::printf("*** self_symlink points to '%s'\n", buffer);

        // Attempt to follow the symlink once
        auto target_node = fs->find(root, buffer);
        show("/self_symlink target", target_node, true);
        delete[] buffer;
    }
    Debug::printf("*** Done with Test 2!\n");

    // Test 3: Symlink cycle (a -> b -> c)
    Debug::printf("*** attempting to find 'link_a'\n");
    auto link_a = fs->find(root, "link_a");
    show("/link_a", link_a, true);

    // Attempt to resolve "link_a" chain
    const int symlink_resolution_limit = 4;
    auto current_node = link_a;
    char symlink_path[256];
    symlink_path[0] = 'l';
    symlink_path[1] = 'i';
    symlink_path[2] = 'n';
    symlink_path[3] = 'k';
    symlink_path[4] = '_';
    symlink_path[5] = 'a';
    symlink_path[6] = '\0';
    for (int i = 0; i < symlink_resolution_limit; i++) {
        if (current_node != nullptr && current_node->is_symlink()) {
            auto sz = current_node->size_in_bytes();
            char* buffer = new char[sz + 1];
            buffer[sz] = '\0';
            current_node->get_symbol(buffer);
            Debug::printf("*** '%s' points to '%s'\n", symlink_path, buffer);

            // Update symlink path
            memcpy(symlink_path, buffer, sz);

            // Attempt to find the next node
            current_node = fs->find(root, buffer);
            if (current_node == nullptr) {
                Debug::printf("***      target does not exist\n");
                break;
            }
            show(buffer, current_node, true);
            delete[] buffer;
        } else {
            break;
        }
    }

    Debug::printf("*** Done with Test 3!\n");

    //Test 4: If you use a class variable to check if you 
    //should keep recursing through the direct pointers/singly indirect pointers/etc,
    //you better make sure to protect that variable from multiple threads.
    uint32_t sum = 0;
    StrongPtr<Barrier> barrier = StrongPtr<Barrier>::make(101);
    for (int i = 0; i < 100; i++) {
        thread([&root, &sum, &barrier](){
            sum += root->entry_count();
            barrier->sync();
        });
    }
    barrier->sync();
    Debug::printf("*** sum: %d\n", sum);

    Debug::printf("*** Done with Test 4!\n");
}

