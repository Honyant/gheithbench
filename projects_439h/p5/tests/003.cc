#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"

/*

This test is a simple (and one of the first) cases that tests a bit of caching and a bit of the new spec for find. 
I make sure you have a sufficiently large cache, but also not too large of a cache. I also
am (currently) the only test that tests 4096 block size, so make sure you don't hardcode anything 
anywhere (especially in your cache). This test isn't designed to be hard but rather to validate 
your cache and find implementation. I don't test symlinks along find path traversal, 
meaning you can test your find without symlinks here.

In all, I test:
1) I test that you at least have a cache (last used should pass the first part of my test)
2) I test you have a sufficiently large cache (something better than just last used)
3) I test the new find spec without symlinks so that you can easily debug the normal cases


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
    auto ide = StrongPtr<Ide>::make(1, 10); //we set our device read to something astronomical, meaning that if you don't cache at all you will fail
    
    // We expect to find an ext2 file system there
    auto fs = StrongPtr<Ext2>::make(ide);

    Debug::printf("*** block size is %d\n",fs->get_block_size());
    Debug::printf("*** inode size is %d\n",fs->get_inode_size());

    {
        // get "/"
        auto root = fs->root;
        auto find1 = fs->find(root, "test_file"); //this should be fine
        Debug::printf("*** you found test_file, is it correct? \n");
        show("/test_file", find1, true);
        auto find2 = fs->find(root, "test_file"); 
        Debug::printf("*** you found a cached value (maybe), is it correct? \n");
        show("/test_file", find2, true);
        auto find3 = fs->find(root, "test_file"); 
        Debug::printf("*** final check of the cached value, is it correct? \n");
        show("/test_file", find3, true);

        for(int i = 0; i < 100; i++){
            auto find3 = fs->find(root, "test_file"); //do it some more now that we're sure it's correct, these should all be memory accesses not disk
        }

        Debug::printf("*** congratulations, your cache does something! Let's test it some more \n"); //but you can get away with last used
    }
    

    //let's see if you're using last used or something a bit better
    {
        ide = StrongPtr<Ide>::make(1, 25);
        fs = StrongPtr<Ext2>::make(ide);

        auto root = fs->root;
        auto find1 = fs->find(root, "test_file"); //cache stores this location
        show("/test_file", find1, true);
        auto find2 = fs->find(root, "test_file2"); //we store somewhere else now, make you store something else in your cache
        show("/test_file2", find2, true);
        auto find3 = fs->find(root, "test_file"); //you should still be able to read this value from the cache, if not you time out :(
        show("/test_file", find3, true);

        Debug::printf("*** You use something more sophisticated than last stored!\n");
    }

    //let's test a few more patterns, making sure you have a sufficiently large cache

    {
        ide = StrongPtr<Ide>::make(1, 25);
        fs = StrongPtr<Ext2>::make(ide);

        auto root = fs->root;
        auto find1 = fs->find(root, "test_file"); //cache stores this location
        show("/test_file", find1, true);
        auto find2 = fs->find(root, "test_file2"); //we store somewhere else now, make you store something else in your cache
        show("/test_file2", find2, true);
        auto find3 = fs->find(root, "test_file3"); //we store somewhere else now, make you store something else in your cache
        auto find4 = fs->find(root, "test_file4"); //we store somewhere else now, make you store something else in your cache
        auto find5 = fs->find(root, "test_file5"); //we store somewhere else now, make you store something else in your cache

        //now, we have displaced at least 6 things from your cache, meaning this will be very slow if you have a small cache

        auto find7 = fs->find(root, "test_file"); //you should still be able to read this value from the cache, if not you time out :(
        show("/test_file", find7, true);

        Debug::printf("*** You use something more sophisticated than last stored!\n");
    }
   

    //Now we move onto find, here we validate relative ahd absolute finds for non symlinks

    {
        ide = StrongPtr<Ide>::make(1, 3); //back to normal time :)
        fs = StrongPtr<Ext2>::make(ide);

        auto root = fs->root;

        auto find1 = fs->find(root, "/");
        show("/", find1, true);

        find1 = fs->find(root, "."); //test this doesn't break things with string parsing (streq for . is hard)
        show(".", find1, true);

        find1 = fs->find(root, "data/..");
        show("data/..", find1, true);

        find1 = fs->find(root, "/data/.."); //these should be the same
        show("/data/..", find1, true);

        find1 = fs->find(root, "data/data1/hidden.txt"); //we make sure you can do longer paths
        show("data/data1/hidden.txt", find1, true);

        auto find2 = fs->find(root, "data");
        auto find3 = fs->find(find2, "data1/hidden.txt"); //we make sure you don't always read things from the root
        show("data/data1/hidden.txt", find1, true);
    }

    Debug::printf("Congratulations! Test over. \n");
    
}

