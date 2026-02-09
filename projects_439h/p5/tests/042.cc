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
            Debug::printf("strlen check %d\n", K::strlen(buffer));
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
   
    // get "/"
    auto root = fs->root;
    show("/",root,true);

    //  get "/abcde"
    auto abcde = fs->find(root,"abcde");
    CHECK(root->number != abcde->number);
    show("/abcde",abcde,true);

    auto abcd_folder = fs->find(abcde,"abcd");
    auto abcd_file = fs->find(root,"abcd");
    auto abcdtxt_file = fs->find(root,"abcd.txt");

    CHECK(abcd_folder->number != abcd_file->number);
    CHECK(abcdtxt_file->number != abcd_file->number);

    // get "/g"
    auto a = fs->find(root,"a");
    auto b = fs->find(root,"b");
    auto c = fs->find(root,"c");
    auto d = fs->find(root,"d");
    auto e = fs->find(root,"e");
    auto f = fs->find(root,"f");
    auto g = fs->find(root,"g");
    for(int i = 0; i < 100; i++) {
        g = fs->find(root,"g");
    }
    show("/g",g,true);


    // get "/not_there"
    auto not_there = fs->find(root,"not_there");
    for(int i = 0; i < 100; i++) {
        not_there = fs->find(root,"not_there");
        auto not_there_a = fs->find(a,"not_there");
        auto not_there_b = fs->find(b,"not_there");
        auto not_there_c = fs->find(c,"not_there");
        auto not_there_d = fs->find(d,"not_there");
        auto not_there_e = fs->find(e,"not_there");
    }
    show("/not_there",not_there,true);

    CHECK(root->number == fs->find(a,"..")->number);
    CHECK(a->number == fs->find(b,"..")->number);
    CHECK(b->number == fs->find(c,"..")->number);
    CHECK(c->number == fs->find(d,"..")->number);
    CHECK(d->number == fs->find(e,"..")->number);
    CHECK(e->number == fs->find(f,"..")->number);
    CHECK(f->number == fs->find(g,"..")->number);
    Debug::printf("*** done!");

}

