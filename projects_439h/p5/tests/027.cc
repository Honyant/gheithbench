#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"

// This test mainly tests your caching system for your GDT
// and repeated reads from the same file


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
    // IDE must be device #1
    // latency_ms is 50 (0.05 seconds), to stress the difference between
    // memory and disk reads. If all your reads are from the disk, this
    // test will timeout.
    auto ide = StrongPtr<Ide>::make(1, 50);
    
    // We expect to find an ext2 file system there
    auto fs = StrongPtr<Ext2>::make(ide);

    Debug::printf("*** block size is %d\n",fs->get_block_size()); // 1024
    Debug::printf("*** inode size is %d\n",fs->get_inode_size()); // 128
    
    //
    // Part 1: Caching Checks
    // 
    // We're gonna start with a simple check that you can cache a single file
    // To pass this, all you need to do is keep the most recent read output
    // in memory, so all the repeated reads of the same file don't cause
    // unnecessary disk reads.
    //
    
    auto root = fs->root;
    // disk read 
    StrongPtr<Node> disk_read_of_easy_cache = fs->find(root, "easy_cache");
    show("/easy_cache", disk_read_of_easy_cache, true);
    
    // memory read/cache hit (just to see that the output is still equal)
    StrongPtr<Node> memory_read_of_easy_cache = fs->find(root, "easy_cache");
    memory_read_of_easy_cache = fs->find(root, "easy_cache");
    show("/easy_cache", memory_read_of_easy_cache, true);
    
    // simple cache loop
    // will take 50 seconds here if you don't cache
    for(int i = 0; i < 1000; i++){
        memory_read_of_easy_cache = fs->find(root, "easy_cache");
        CHECK(disk_read_of_easy_cache->number == memory_read_of_easy_cache->number);
    }
    
    auto larger_cache = fs->find(root, "larger_cache");
    
    // Instead of most recently used, can you refrence the last few (10) read values?
    // all disk reads (0.5 seconds)
    for (int i = 0; i < 10; i++) {
        // auto hard_cache_i = fs->find(root, "hard_cache_" + i);
        char filename[] = "hard_cache_i";
        filename[11] = i + '0';
        auto hard_cache_i = fs->find(larger_cache, filename);
        show(filename, hard_cache_i, true);
    }
    
    // all memory reads
    for (int i = 0; i < 1000; i++) {
        char filename[] = "hard_cache_i";
        filename[11] = (i % 10) + '0';
        auto hard_cache_i = fs->find(larger_cache, filename);
    }
    
    // The only real bound here is the memory of our kernel itself, which is 
    // 8 << 20 bytes (8388 KB). It's a little ridiculous to expect that one should even 
    // come close to this upper bound for a disk cache.
    //
    // Because there's no further guidance on how much memory should be reserved for your
    // kernel implemetation, I'm going to take a conservative estimate and assume that
    // 1% of the kernel memory (83 KB) should be dedicated to the cache.
    //
    // We now test this!
    //
    
    for (int i = 0; i < 1000; i++) {
        char filename[] = "hard_cache_i";
        filename[11] = (i % 10) + '0';
        auto hard_cache_i = fs->find(larger_cache, filename);
        fs->find(root, "75kb_file.txt");
    }
    
    Debug::printf("*** cache test passed!\n");

}

