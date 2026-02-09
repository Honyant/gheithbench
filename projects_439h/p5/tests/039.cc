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

// this test case tests various different cases for find() involving nested directories and files
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

   // test 1: retrieving files directly below root
   auto file1 = fs->find(root,"file1.txt");
   CHECK(root->number != file1->number);
   show("/file1.txt",file1,true);

   auto file2 = fs->find(root,"file2.txt");
   CHECK(root->number != file2->number);
   show("/file2.txt",file2,true);
   
   auto file3 = fs->find(root,"file3.txt");
   CHECK(root->number != file3->number);
   show("/file3.txt",file3,true);

   // test 2: retrieving files multiple levels down the root, also ensuring starting with "/" doesn't break find
   auto file4 = fs->find(root,"/dir1a/dir1b/file4.txt");
   CHECK(root->number != file4->number);
   show("/dir1a/dir1b/file4.txt",file4,true);

   auto file5 = fs->find(root,"dir1a/dir1b/dir1c/file5.txt");
   CHECK(root->number != file5->number);
   show("/dir1a/dir1b/dir1c/file5.txt",file5,true);
   
   auto file6 = fs->find(root,"/dir1a/dir1b/dir1d/file6.txt");
   CHECK(root->number != file6->number);
   show("/dir1a/dir1b/dir1d/file6.txt",file6,true);

   auto file7 = fs->find(root,"dir1a/dir1b/dir1d/dir1e/dir1f/file7.txt");
   CHECK(root->number != file7->number);
   show("dir1a/dir1b/dir1d/dir1e/dir1f/file7.txt",file7,true);

    // test 3: retrieving files based on relative paths (not from root) and also using .. and . and adding "/" to the end of directory to ensure it is treated as /.
   auto dir1b = fs->find(root, "/./dir1a/dir1b/");
   show("/dir1a/dir1b/dir1c/file5.txt",fs->find(dir1b,"dir1c/file5.txt"),true);
   show("/dir1a/dir1b/dir1c/file5.txt",fs->find(dir1b,"dir1c/../dir1c/file5.txt"),true);

   auto dir1a = fs->find(root, "dir1a/../dir1a/dir1b/..");
   show("/dir1a/dir1b/dir1d/dir1e/dir1f/file7.txt",fs->find(dir1a,"dir1b/dir1d/dir1e/dir1f/file7.txt"),true);
   show("/dir1a/dir1b/dir1d/dir1e/dir1f/file7.txt",fs->find(dir1a,"/dir1a/dir1b/dir1d/dir1e/dir1f/file7.txt"),true); // ensuring the first "/" makes find() traverse from root instead of dir1a

}