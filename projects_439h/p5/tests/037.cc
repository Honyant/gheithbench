#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"

// This test case tests that you free any and all buffers
// you use to store the directory entries and resolved symlinks
// when calling "find"

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
        delete[] buffer;
    } else {
        Debug::printf("***    is of type %d\n",node->get_type());
    }
}


void kernelMain(void) {
    // IDE device #1 - No latency
    auto ide = StrongPtr<Ide>::make(1, 0);
    auto fs = StrongPtr<Ext2>::make(ide);

    //Config info
    Debug::printf("*** block size is %d\n",fs->get_block_size());
    Debug::printf("*** inode size is %d\n",fs->get_inode_size());
   
   //Allocate 4 mb junk
   char* junk = (char*) malloc(4 << 20);

   // Get root
   auto root = fs->root;
   show("/",root,true);

   // Get symlink
   auto back_to_dir = fs->find(root,"link_to_dir_1");
   show("/link_to_dir_1",back_to_dir,true);

   // Resolve symlink
   auto dir1 = fs->find(root,"link_to_dir_1/");
   show("/link_to_dir_1/", dir1,true);
    
   Debug::printf("*** If everything above matches the ok file up until this point, your symbolic links resolve correctly and you are now ready for the freeing test\n");
   Debug::printf("*** (If nothing appears after this, you didn't make it) Get ready to make the jump... 1... 2... 3... JUMP!\n");
   

   //*************************************Now time to stress memory freeing in find!******************************************
   //You need to make sure you free your buffer to read in the inode and also the buffer created to store the resolved symlink

   for (uint32_t i = 0; i < 500; i++) {
        dir1 = fs->find(root,"link_to_dir_1/"); //Each find typically allocates buffers 
   }
   Debug::printf("*** Wow congratulations, you made it to the other side and didn't fall into the chasm\n");
   
   //No memory leaks
   free(junk);
}

