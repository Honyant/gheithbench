#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"
#include "debug.h"

// This test case tests a lot of nexted directories
/* Called by one core */
void kernelMain(void) {
    // IDE device #1
    auto ide = StrongPtr<Ide>::make(1, 3);
    
    // We expect to find an ext2 file system there
    auto fs = StrongPtr<Ext2>::make(ide);

    Debug::printf("*** block size is %d\n",fs->get_block_size());
    Debug::printf("*** inode size is %d\n",fs->get_inode_size());

    // get "/"
    auto current = fs->root;
    if (!current->is_dir()) {
        Debug::printf("*** Failed root check\n");
    }

    for(int i = 0; i < 10; i++){
        current = fs->find(current, "box");
        if (!current->is_dir()) {
            Debug::printf("*** Failed at iteration %d\n", i);
        }
    }

    auto file = fs->find(current, "free_ducklings");
    if (!file->is_file()) {
        Debug::printf("*** Failed duckling check\n");
    }

    for(int i = 0; i < 10; i++){
        current = fs->find(current, "box");
        if (!current->is_dir()) {
            Debug::printf("*** Failed at iteration %d\n", i + 10);
        }
    }

    file = fs->find(current, "bevo");
    if (!file->is_file()) {
        Debug::printf("*** Failed bevo check\n");
    }

    Debug::printf("*** All Done !  >(.)__ \n");
    Debug::printf("***              (___/ \n");

}

