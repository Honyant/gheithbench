#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"

/*
This test tests different symlink scenarios, including resolving vs. not resolving symlinks,
chained symlinks, and a symlink to the root directory.
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

void printFile(StrongPtr<Node> node) {
    auto sz = node->size_in_bytes();
    char* fileContent = new char[sz+1];
    node->read_all(0, sz, fileContent);
    fileContent[sz] = '\0';
    Debug::printf("*** %s\n", fileContent);
}

/* Called by one core */
void kernelMain(void) {
    auto ide = StrongPtr<Ide>::make(1, 3);
    auto fs = StrongPtr<Ext2>::make(ide);
    
    // root
    auto root = fs->root;
    show("root", root, false);

    // basic directory
    auto dir1 = fs->find(root, "dir1");
    show("/root/dir1", dir1, true);

    // basic text file
    auto firsttxt = fs->find(dir1, "first.txt");
    show ("/dir1/first.txt", firsttxt, true);
    printFile(firsttxt);

    // symlink to a dir - relative path
    auto symlink1 = fs->find(root, "/dir2/symlinkToDir1");
    show ("/dir2/symlinkToDir1", symlink1, true);
    
    // symlink which needs to be resolved - directory
    auto symlink2 = fs->find(root, "/dir2/symlinkToDir1/");
    show ("/dir2/symlinkToDir1/", symlink2, true);
    
    // symlink which needs to be resolved - file
    auto symlink3 = fs->find(root, "/dir2/symlinkToDir1/relative.txt");
    show ("/dir2/symlinkToDir1/relative.txt", symlink3, true);
    printFile(symlink3);

    // chained symlink - to a directory, needs to be resolved
    auto chainedSymlink = fs->find(root, "/dir3/chainedSymlink/");
    show ("/dir3/chainedSymlink/", chainedSymlink, true);
    
    // chained symlink - to a file
    auto chainedSymlink2 = fs->find(root, "/dir3/chainedSymlink/chain.txt");
    show ("/dir3/chainedSymlink/chain.txt", chainedSymlink2, true);
    printFile(chainedSymlink2);

    // symlink to the root
    auto rootSymlink = fs->find(root, "/dir3/rootSymlink/dir3/rootSymlink/dir3/root.txt");
    show ("/dir3/rootSymlink/dir3/rootSymlink/dir3/root.txt", rootSymlink, true);
    printFile(rootSymlink);
}

