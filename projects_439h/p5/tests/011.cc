#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"

// This test case is designed to verify some of the weirder functionality of finds under the new spec
// I have endeavored to make it simple to debug, so it should be a good "first test case" to look at when fully implementing find

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

// This test case is designed to verify some of the weirder functionality of finds under the new spec
// I have endeavored to make it simple to debug, so it should be a good "first test case" to look at when fully implementing find

/* Called by one core */
void kernelMain(void) {
    // IDE device #1
    auto ide = StrongPtr<Ide>::make(1, 3);
    
    // We expect to find an ext2 file system there
    auto fs = StrongPtr<Ext2>::make(ide);


    // Leaving these to make the information obvious
    Debug::printf("*** block size is %d\n",fs->get_block_size());
    Debug::printf("*** inode size is %d\n",fs->get_inode_size());
   
   // get "/""
   auto root = fs->root;
   show("/",root,true);

   // Test one: basic find of single file
   Debug::printf("*** Test one: basic find of single file\n");
   auto funFile = fs->find(root,"funFile");
   CHECK(root->number != funFile->number);
   show("/funFile",funFile,true);

    // Test two: two level find
   Debug::printf("*** Test two: two level find\n");
   auto data = fs->find(root,"data/data.txt");
   CHECK(root->number != data->number);
   show("/data/data.txt",data,true);

   // Test three: three level find
   Debug::printf("*** Test three: three level find\n");
   auto subData = fs->find(root,"data/subData/subData.txt");
   CHECK(root->number != subData->number);
   show("/data/subData/subData.txt",subData,true);

   // Test four: find ending with a /
   Debug::printf("*** Test four: find ending with a /\n");
   auto subDataFolder = fs->find(root,"data/subData/");
   CHECK(root->number != subDataFolder->number);
   show("/data/subData/",subDataFolder,true);

   // Test five: find starting with a /
   Debug::printf("*** Test five: find starting with a /\n");
   auto funFile2 = fs->find(subDataFolder,"/funFile");
   CHECK(root->number != funFile2->number);
   show("/funFile",funFile2,true);


   // Test six: find following a symLink
   Debug::printf("*** Test six: find following a symLink\n");
   auto subData2 = fs->find(root,"linkToSubData/subData.txt");
   CHECK(root->number != subData2->number);
   show("/linkToSubData/subData.txt",subData2,true);

   // Test seven: find following a symLink, with a / at the end
   Debug::printf("*** Test seven: find following a symLink, with a / at the end\n");
   auto subDataFolder2 = fs->find(root,"linkToSubData/");
   CHECK(root->number != subDataFolder2->number);
   show("/linkToSubData/",subDataFolder2,true);

   // Test eight: find following a symlink whose path contains a symlink
   Debug::printf("*** Test eight: find following a symlink whose path contains a symlink\n");
   auto subData3 = fs->find(root,"linkToLinkToSubData/subData.txt");
   CHECK(root->number != subData3->number);
   show("/linkToLinkToSubData/subData.txt",subData3,true);


   Debug::printf("*** Test Complete! Great Job!\n");

}

