#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"
#include "debug.h"
#include "atomic.h"

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

/**
 * A test for limiting cache size, finding, 16 symlink search, caching, and thread safety.
 */
void kernelMain(void) {
    char* to_delete = (char*)malloc(4194304); // Enforce a 1 MB cache max like Gheith mentioned in class
    auto ide = StrongPtr<Ide>::make(1, 3);
    auto fs = StrongPtr<Ext2>::make(ide);

    Debug::printf("*** block size is %d\n",fs->get_block_size());
    Debug::printf("*** inode size is %d\n",fs->get_inode_size());
    
    auto root = fs->root;
    show("/",root,true);

    // Traversing 16 symlinks
    auto goats = fs->find(root, "goat_link16/goats.txt");
    show("goat_link16/goats.txt", goats, true);

    // Reading the same file over and over, stresses caching
    for (int i = 0; i < 1000; i++) {
        auto sz = goats->size_in_bytes();
        auto buffer = new char[sz+1];
        goats = fs->find(root, "goats/goats.txt");
        delete[] buffer;
    }

    Debug::printf("*** You didn't timeout!\n");
    
    // Let's try accessing dot a lot, you can optimize for this by skipping dots and staying on the same node
    goats = fs->find(root, "././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././././goats/goats.txt");
    show("goats/goats.txt", goats, true);

    // Thread safety, let's make multiple reads happen simultaneously
    auto thread_node = fs->find(root, "threads");
    Atomic<uint32_t> full_val {828000 * 3};
    for (uint32_t i = 0; i < 3; i ++) {
        thread([fs, thread_node, &full_val]() mutable {
            char file_name[6];
            file_name[1] = '.';
            file_name[2] = 't';
            file_name[3] = 'x';
            file_name[4] = 't';
            file_name[5] = '\0';
            for(char name = 'A'; name <= 'T'; name++) {
                file_name[0] = name;
                auto file_node = fs->find(thread_node, file_name);
                auto sz = file_node->size_in_bytes();
                auto buffer = new char[sz+1];
                file_node->read_all(0,sz,buffer);
                buffer[sz] = 0;
                for (uint32_t i = 0; i < sz; i++) {
                    CHECK(buffer[i] == 46);
                    full_val.fetch_add(-buffer[i]);
                }
                delete[] buffer;
            }
        });
    }

    while(full_val.get() != 0);

    free(to_delete);

}

