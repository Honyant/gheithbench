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
   
    // get "/" directory
    auto root = fs->root;
    show("/",root,true);
    
    auto file1 = fs->find(root, "file1");
    show("/file1",file1,false);
    
    auto lorem_ispum = fs->find(root, "lorem_ispum");
    show("/lorem_ispum",file1,false);

    Debug::printf("*** the first block and second block of lorem ispum:\n");
    auto block_buffer = new char[fs->get_block_size()];
    lorem_ispum->read_block(0, block_buffer);
    for (uint32_t i = 0; i < fs->get_block_size(); i++) {
        Debug::printf("%c", block_buffer[i]);
    }

    lorem_ispum->read_block(1, block_buffer);
    for (uint32_t i = 0; i < fs->get_block_size(); i++) {
        Debug::printf("%c", block_buffer[i]);
    }
    Debug::printf("\n");
    
    for (int i = 0; i < 2000; i++) {
        lorem_ispum->read_block(0, block_buffer);
        lorem_ispum->read_block(1, block_buffer);
    }

   // tests early stopping in the find method
   auto deeply_nested_file = fs->find(root, "this/is/a/very/deeply/nested/directory/structure/to/test/caching/of/nodes");
   show("....long structure..../nodes", deeply_nested_file, true);
}