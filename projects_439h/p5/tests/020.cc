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
/** 
    This test incrementally gets more difficult to test your
    symlink and your find implementation. It also makes sure
    you don't override data. 
 */
void kernelMain(void) {
    auto ide = StrongPtr<Ide>::make(1, 3);
    auto fs = StrongPtr<Ext2>::make(ide);

    Debug::printf("*** block size is %d\n",fs->get_block_size());
    Debug::printf("*** inode size is %d\n",fs->get_inode_size());
   
   // get "/"
   auto root = fs->root;
   uint32_t rootEntryCount = root->entry_count();
   uint32_t rootLinkCount = root->n_links();
   show("/",root,true);

    // accessing basic file
    Debug::printf("*** Accessing a file at the root level\n");
    auto basic = fs->find(root, "basicFile");
    ASSERT(root->is_dir());    // make sure you don't override any of the parent info after find
    ASSERT(root->entry_count() == rootEntryCount);
    ASSERT(root->n_links() == rootLinkCount);
    show("basicFile", basic, true);

    // find with multiple subdirectories
    Debug::printf("*** Testing find with multiple subdirectories\n");
    auto multi_find_11 = fs->find(root, "dir1/subdir11");
    ASSERT(root->is_dir());    // make sure you don't override any of the parent info after find
    ASSERT(root->entry_count() == rootEntryCount);
    ASSERT(root->n_links() == rootLinkCount);
    uint32_t multi_find_11_entryCount = multi_find_11->entry_count();
    uint32_t multi_find_11_links = multi_find_11->n_links();
    show("/dir1/subdir11", multi_find_11, true);

    auto multi_find_1111 = fs->find(multi_find_11, "subdir111/subdir1111");
    ASSERT(multi_find_11->is_dir());    // make sure you don't override any of the parent info after find
    ASSERT(multi_find_11->entry_count() == multi_find_11_entryCount);
    ASSERT(multi_find_11->n_links() == multi_find_11_links);
    show("/subdir111/subdir1111", multi_find_1111, true);

    auto single_find_1111 = fs->find(root, "dir1/subdir11/subdir111/subdir1111");
    show("/dir1/subdir11/subdir111/subdir1111", single_find_1111, true);
    ASSERT(single_find_1111->number == multi_find_1111->number);

    // symlink to a folder (resolved node is at the root level)
    Debug::printf("*** Testing a symlink to a folder (unresolved)\n");
    auto find_1111_symlink_to_dir1 = fs->find(root, "dir1/subdir11/subdir111/subdir1111/linkToDir1");
    show("dir1/subdir11/subdir111/subdir1111/linkToDir1", find_1111_symlink_to_dir1, true);

    // resolve the symlink
    Debug::printf("*** Testing a symlink to a folder (resolved)\n");
    auto dir1 = fs->find(root, "dir1");
    auto find_1111_symlink_to_dir1_folder = fs->find(root, "dir1/subdir11/subdir111/subdir1111/linkToDir1/");
    show("dir1/subdir11/subdir111/subdir1111/linkToDir1/", find_1111_symlink_to_dir1_folder, true);
    ASSERT(dir1->number == find_1111_symlink_to_dir1_folder->number);
    
    // use a symlink in its own path
    Debug::printf("*** Finding a symlink in its own path\n");
    auto find_1111_symlink_from_1111_symlink = fs->find(root, "dir1/subdir11/subdir111/subdir1111/linkToDir1/subdir11/subdir111/subdir1111/linkToDir1");
    show("linkToDir1/subdir11/subdir111/subdir1111/linkToDir1", find_1111_symlink_from_1111_symlink, true);
    ASSERT(find_1111_symlink_from_1111_symlink->number == find_1111_symlink_to_dir1->number);

    Debug::printf("*** Using a symlink in a path\n");
    auto find_basicFile_using_symlink = fs->find(root, "dir1/subdir11/subdir111/subdir1111/linkToDir1/../basicFile");
    show("dir1/subdir11/subdir111/subdir1111/linkToDir1/../basicFile", find_basicFile_using_symlink, true);
    ASSERT(find_basicFile_using_symlink->number == basic->number);

    // symlink that points to a symlink
    Debug::printf("*** Symlink that points to another symlink\n");
    auto find_basicFile_using_two_symlinks = fs->find(root, "dir1/subdir11/linkToLinkToDir1/../basicFile");
    show("dir1/subdir11/linkToLinkToDir1/../basicFile", find_basicFile_using_two_symlinks, true);
    ASSERT(find_basicFile_using_two_symlinks->number == basic->number);

    // symlink that points to the directory it is in
    Debug::printf("*** Symlink that points to the directory it is in (unresolved)\n");
    auto symlink_subdir1111 = fs->find(root, "dir1/subdir11/subdir111/subdir1111/subdir1111_symlink");
    show("dir1/subdir11/subdir111/subdir1111/subdir1111_symlink", symlink_subdir1111, true);

    Debug::printf("*** Symlink that points to the directory it is in (resolved)\n");
    auto symlink_subdir1111_resolved = fs->find(root, "dir1/subdir11/subdir111/subdir1111/subdir1111_symlink/");
    show("dir1/subdir11/subdir111/subdir1111/subdir1111_symlink/", symlink_subdir1111_resolved, true);
    ASSERT(single_find_1111->number == symlink_subdir1111_resolved->number);

    Debug::printf("*** A few more complicated file paths!\n");
    auto basic_from_subdir1111 = fs->find(root, "dir1/subdir11/subdir111/subdir1111/subdir1111_symlink/linkToDir1/../basicFile");
    ASSERT(basic_from_subdir1111->number == basic->number);

    auto basic_from_subdir1111_double = fs->find(root, "dir1/subdir11/subdir111/subdir1111/subdir1111_symlink/linkToDir1/subdir11/linkToLinkToDir1/../basicFile");
    ASSERT(basic_from_subdir1111_double->number == basic->number);

    Debug::printf("*** All done!\n");
}

