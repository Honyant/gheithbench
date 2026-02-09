#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"

void show(const char* name, StrongPtr<Node> node) {

    Debug::printf("*** looking at %s\n",name);
    ASSERT(node->is_file());

    Debug::printf("***      is a file\n");
    auto sz = node->size_in_bytes();
    Debug::printf("***      contains %d bytes\n",sz);
    Debug::printf("***      has %d links\n",node->n_links());
    auto buffer = new char[sz+1];
    buffer[sz] = 0;
    auto cnt = node->read_all(0,sz,buffer);
    CHECK(sz == cnt);
    CHECK(K::strlen(buffer) == cnt);
    // can't just print the string because there is a 1000 character limit
    // on the output string length.
    Debug::printf("***      ");
    for (uint32_t i=0; i<cnt; i++) {
        Debug::printf("%c",buffer[i]);
        if (buffer[i] == '\n' && i != cnt-1) Debug::printf("***      ");
    }
    delete[] buffer;
    Debug::printf("\n");
}

/* Called by one core */
void kernelMain(void) {
    // IDE device #1
    auto ide = StrongPtr<Ide>::make(1, 3);
    
    // Verify that the cache isn't created multiple times. 
    // Will fail for caches at least 100K (which is ... 25-ish lines?)
    StrongPtr<Ext2> fs;
    const int MAX_MAKE = 100;
    const int MAX_QUERY = 1000;
    for (int i = 0; i < MAX_MAKE; i++) {
        fs = StrongPtr<Ext2>::make(ide);
    }

    Debug::printf("*** block size is %d\n",fs->get_block_size());
    Debug::printf("*** inode size is %d\n",fs->get_inode_size());

    auto root = fs->root;

    // Verify that a cache exists
    for (int i = 0; i < MAX_QUERY; i++) {
        const char* path;
        switch (i % 4) {
            case 0: path = "/sunshine"; break;
            case 1: path = "/rainbow"; break;
            case 2: path = "/white"; break;
            case 3: path = "/pony"; break;
            default: ASSERT(false);
        }   
        auto node = fs->find(root, path);
        if (i >= MAX_QUERY-4) show(path, node);
    }
    Debug::printf("*** bing chilling\n");
}

