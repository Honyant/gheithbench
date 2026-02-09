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

// This testcases tests functionality related to symbolic links

/* Called by one core */
void kernelMain(void) {
    // IDE device #1
    auto ide = StrongPtr<Ide>::make(1, 3);
    
    // We expect to find an ext2 file system there
    auto fs = StrongPtr<Ext2>::make(ide);

    Debug::printf("*** block size is %d\n",fs->get_block_size());
    Debug::printf("*** inode size is %d\n",fs->get_inode_size());
   
   auto root = fs->root;

   // Test 1: retrieve symlink
   Debug::printf("*** Test 1: retrieve symlink\n");
   auto data1 = fs->find(root,"helloSym");
   CHECK(root->number != data1->number);
   show("/helloSym",data1,true);

   // Test 2: Go through symlink to a directory
   Debug::printf("*** Test 2: go through symlink to directory\n");
   auto dataDir = fs->find(root,"dataDirSym/");
   CHECK(root->number != dataDir->number);
   show("/dataDirSym/",dataDir,true);

   // Test 3: Go through 2 layers of symlinks
   Debug::printf("*** Test 3: go through 2 symlinks to data\n");
   auto data1_2 = fs->find(root,"dataDirSym/data1DirSym/data1.txt");
   CHECK(root->number != data1_2->number);
   show("/dataDirSym/data1DirSym/data1.txt",data1_2,true);

}

