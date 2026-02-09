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

/* Called by one core */
void kernelMain(void) {
    // this test focuses on names to make sure they're not 

    auto ide = StrongPtr<Ide>::make(1, 3);
    auto fs = StrongPtr<Ext2>::make(ide);

    Debug::printf("*** block size is %d\n",fs->get_block_size());
    Debug::printf("*** inode size is %d\n",fs->get_inode_size());

    auto root = fs->root;
    show("/",root,true);

    // test very long file name
    auto veryLongFileTest = fs->find(root, "verylongfiletestverylongfiletestverylongfiletestverylongfiletestverylongfiletestverylongfiletestverylongfiletestverylongfiletestverylongfiletestverylongfiletestverylongfiletestverylongfiletestverylongfiletestverylongfiletestverylongfiletestverylongfiletes");
    show("/verylongfiletest", veryLongFileTest, true);

    // test too long file name
    auto tooLongFileTest = fs->find(root, "tooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletesttooolongfiletest");
    show("/toolongfiletest", tooLongFileTest, true);

    // test file that doesnt exist
    auto spaceNameFile = fs->find(root,"s p a c e");
    show("/s p a c e",spaceNameFile,true);

    // test file that doesnt exist
    auto doesNotExist = fs->find(root,"doesnotexist");
    show("/doesnotexist",doesNotExist,true);

    // test nested directories
    auto nestedDir = root; 
    for(int i = 49; i <= 54; i++) {
        char c[2]; // make string
        c[0] = i;
        c[1] = 0;
        nestedDir = fs->find(nestedDir, c);
    }
    auto nestedFile = fs->find(nestedDir, "nestedfile");
    show("/1/2/3/4/5/6/nestedfile",nestedFile,true);

    // test silly
    auto emojiFile = fs->find(root, "😊😊😊😊");
    show("/😊😊😊😊",emojiFile,true);


}

