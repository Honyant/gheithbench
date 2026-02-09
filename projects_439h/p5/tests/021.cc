#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"

// CITATION: Copied from t0 with consent of TAs
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
 * This test makes sure you follow the updated find spec in #339
 * Make sure to read it closely to make sure you do it by the book
 */
void kernelMain(void) {
    // IDE device #1
    auto ide = StrongPtr<Ide>::make(1, 3);
    auto fs = StrongPtr<Ext2>::make(ide);

   auto root = fs->root;

   /**
    * Test: You should follow at least 16 sym links before panicking
    * This will follow 16 symlinks before ending. Make sure you keep track 
    * correctly so you don't end early.
    */
   show("/sym_dir/link_1/ans.txt", fs->find(root,"sym_dir/link_1/ans.txt"),true);
   
   /**
    * Test: A slash after a symlink means you should traverse it
    * Make sure you check if there is a slash left at the end 
    */
   show("/sym_dir/link_16/", fs->find(root,"sym_dir/link_16/"),true);

}

