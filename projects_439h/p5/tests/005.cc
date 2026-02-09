#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"

void initalizefs(StrongPtr<Ide> ide) {
    auto fs = StrongPtr<Ext2>::make(ide);
}

/* Called by only one core */
void kernelMain(void) {
    Debug::printf("*** start\n");
    
    // IDE device #1
    auto ide = StrongPtr<Ide>::make(1, 10);

    // initialize and delete 70 ext2s
    for (uint32_t i = 0; i < 50; i++)
    {
        // initalizefs(ide);
    }
    
    auto fs = StrongPtr<Ext2>::make(ide);

    // whatever this is, it tests read_block doesn't break your code
    auto root = fs->root;
    uint32_t bytes_to_read = 10;
    uint32_t *arr = new uint32_t[bytes_to_read];
    root->read_all(0, bytes_to_read, (char *) arr);
    Debug::printf("***");
    for (uint32_t i = 0; i < bytes_to_read; i++) {
        Debug::printf(" %d", arr[i]);
    }
    Debug::printf("\n");

    // Basic count test
    uint32_t count = root->entry_count();
    Debug::printf("*** count: %d\n", count);

    // Basic find test
    auto dir1 = fs->find(fs->root,"dir1");
    Debug::printf("*** newroot type: %d\n", dir1->get_type());
    Debug::printf("*** newroot entry count: %d\n", dir1->entry_count());

    // char *buffer = new char[1024];
    // dir1->read_block(0, buffer);

    // Basic symlink test
    auto symlink = fs->find(fs->root,"./../dir2/goodbye");
    char *buffer = new char[1000];
    symlink->get_symbol(buffer);
    Debug::printf("*** symlink contents: %s\n", buffer);
    

    // Change the root of the file system
    fs->root = dir1;
    ASSERT(!(fs->root == root)); // assert root has changed
    ASSERT(fs->root == dir1);

    // // see if I can get the new root's parent
    auto oldroot = fs->find(fs->root,"..");
    ASSERT((oldroot->number == root->number));

    // annoying recursive case
    fs->root = root;
    auto data = fs->find(fs->root, "./dir1/../dir2/../dir1/../dir2/../dir1/../dir2/../dir1/../dir2");
    ASSERT(data->entry_count() == 4);
    auto datafile = fs->find(data, "/hello");
    ASSERT(datafile->is_file());

    Debug::printf("*** end\n");
}

