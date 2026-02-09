#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"
#include "blocking_lock.h"
#include "atomic.h"
#include "barrier.h"
#include "shared.h"

const uint32_t num_threads = 10;
BlockingLock lock{};
StrongPtr<Barrier> barriers[num_threads + 1];
const char* filenames[num_threads] = {"song0", "song1", "song2", "song3", "song4", "song5", "song6", "song7", "song8", "song9"};
StrongPtr<Ext2> fs {};

void show(const char* name, StrongPtr<Node> node, bool show) {
    LockGuard g{lock};

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

void show(StrongPtr<Node> node) {
    auto sz = node->size_in_bytes();
    auto buffer = new char[sz+1];
    buffer[sz] = 0;
    auto cnt = node->read_all(0,sz,buffer);
    ASSERT(sz == cnt);
    //CHECK(K::strlen(buffer) == cnt);
    // can't just print the string because there is a 1000 character limit
    // on the output string length.
    //for (uint32_t i=0; i<cnt; i++) {
    //    Debug::printf("%c",buffer[i]);
    //}
    Debug::printf("%s\n", buffer);
    delete[] buffer;
    Debug::printf("\n");
}

/* Called by one core */
void kernelMain(void) {
    // IDE device #1
    auto ide = StrongPtr<Ide>::make(1, 5);
    
    // We expect to find an ext2 file system there
    fs = StrongPtr<Ext2>::make(ide);
    Debug::printf("fs = %p\n", fs.operator->());
    auto root = fs->root;
    show("/",root,true);

    for (uint32_t i = 0; i < num_threads; i++) {
        barriers[i] = new Barrier(num_threads);
    }
    barriers[num_threads] = new Barrier(num_threads+1);

    for (uint32_t i = 0; i < num_threads; i++) {
        thread([i, root] () {

            auto myfs = fs.operator->();

            auto my_file = myfs->find(root, filenames[i]);

            for (uint32_t b = 0; b < i; b++) {
                //Debug::printf("*** %d syncing barrier %d\n", i, b);
                barriers[b]->sync();
            }

            show(my_file);
            
            for (uint32_t b = i; b < num_threads; b++) {
                //Debug::printf("*** %d syncing barrier %d\n", i, b);
                barriers[b]->sync();
            }
            barriers[num_threads]->sync();
        });
    }

    barriers[num_threads]->sync();
    Debug::printf("*** done!\n");
}

