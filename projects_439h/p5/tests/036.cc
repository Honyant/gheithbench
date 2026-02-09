#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"

/*
The primary goal of this test case is to test CACHING. It is NOT testing
symlinks or the functions associated with that... ONLY CACHING!!!
This test case essentially just creates a file that is 10 directories
deep and then asks you to find that file 100 times. This test case will
also check if you use your memory efficiently. You might need to check
if you delete buffers at the right times so you do not run out of 
memory or encounter a stack overflow D: ... 
Best of luck!!
*/

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
    // IDE device #1
    auto ide = StrongPtr<Ide>::make(1, 3);
    
    // We expect to find an ext2 file system there
    auto fs = StrongPtr<Ext2>::make(ide);

    Debug::printf("*** block size is %d\n",fs->get_block_size());
    Debug::printf("*** inode size is %d\n",fs->get_inode_size());

    // get "/"
    auto root = fs->root;
    show("/",root,true);
    Debug::printf("*** looking for our bouquet\n");
    auto bouquet = fs->find(root,"./lavender/rose/peony/tulip/iris/daisy/sunflower/lily/hyacinth/orchid/bouquet.txt");
    show("/lavender/rose/peony/tulip/iris/daisy/sunflower/lily/hyacinth/orchid/bouquet.txt",bouquet,true);

    Debug::printf("*** found one! do we have the time to find another?\n");

    for (int i = 0; i < 100; i++)
    {
        // get "/lavender/rose/peony/tulip/iris/daisy/sunflower/lily/hyacinth/orchid/bouquet.txt"
        auto bouquet = fs->find(root,"./lavender/rose/peony/tulip/iris/daisy/sunflower/lily/hyacinth/orchid/bouquet.txt");
    }
    show("/lavender/rose/peony/tulip/iris/daisy/sunflower/lily/hyacinth/orchid/bouquet.txt",bouquet,true);
    Debug::printf("*** Yay we found it! Isn't it pretty?\n");
}

