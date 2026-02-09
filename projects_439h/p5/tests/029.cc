#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"
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
            Debug::printf("*** ");
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
    
    // check that recursive directory finding works
    auto root = fs->root;
    auto cinnamoroll = fs->find(root,"data/data2/data3/data4/data5/cinnamoroll.txt");
    CHECK(root->number != cinnamoroll->number);
    show("/cinnamoroll.txt",cinnamoroll,true);

    // check that finding something that doesn't exist works
    auto not_found = fs->find(root, "data/kuromi");
    show("/kuromi", not_found, true);

    // check that long symlink names work
    auto character_list = fs->find(root,"character_list");
    show("/character_list",character_list,true);

    // test one indirection
    auto sanrio_article = fs->find(root, "sanrio_article.txt");
    show("/sanrio_article.txt", sanrio_article, false);
    char* c = new char[1024];
    sanrio_article->read_block(50, c);
    Debug::printf("*** ");
    for (int i = 0; i < 1024; i++) {
        Debug::printf("%c", *(c + i));
        if (*(c + i) == '\n') {
            Debug::printf("*** ");
        }
    }
    Debug::printf("\n");

    // use threads to make sure you don't have race conditions
    auto b = new Barrier(6);
    for (int i = 1; i <= 5; i++) {
        thread([i, root, &fs, b] {
            char* char_str = new char[24];
            ::memcpy(char_str, "sanrio_world/character", 22); 
            char_str[22] = '0' + i;
            char_str[23] = 0;
            auto character = fs->find(root, char_str);
            sleep(i);
            show(char_str, character, true);
            b->sync();
        });
    }
    b->sync();
}

