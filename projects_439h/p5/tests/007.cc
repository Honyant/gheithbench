#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"
#include "heap.h"

/* 
* This test case ensures that your code is free of memory leaks, which is important in a long-running program with many filesystem accesses.
* Along the way, I test many of the access operations a user would want to perform, checking that your filesystem implementation behaves as expected.
* Since I am not testing how well you reduce disk accesses (via caching or otherwise), I set the IDE latency to 0.
* The test also mildly stresses the effeiciency of your string parsing in find(), although recursive, tokenizing, and pure iterative approaches can all suffice.
*/


// essentially the same as in t0 but with tab characters
void show(const char* name, StrongPtr<Node> node, bool show) {

    Debug::printf("*** looking at %s\n",name);

    if (node == nullptr) {
        Debug::printf("*** \tdoes not exist\n");
        return;
    } 

    if (node->is_dir()) {
        Debug::printf("*** \tis a directory\n");
        Debug::printf("*** \tcontains %d entries\n",node->entry_count());
        Debug::printf("*** \thas %d links\n",node->n_links());
    } else if (node->is_symlink()) {
        Debug::printf("*** \tis a symbolic link\n");
        auto sz = node->size_in_bytes();
        Debug::printf("*** \tlink size is %d\n",sz);
        auto buffer = new char[sz+1];
        buffer[sz] = 0;
        node->get_symbol(buffer);
        Debug::printf("*** \t=> %s\n",buffer);
        delete[] buffer;
    } else if (node->is_file()) {
        Debug::printf("*** \tis a file\n");
        auto sz = node->size_in_bytes();
        Debug::printf("*** \tcontains %d bytes\n",sz);
        Debug::printf("*** \thas %d links\n",node->n_links());
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
        Debug::printf("*** \tis of type %d\n",node->get_type());
    }
}


void kernelMain(void) {
    Debug::printf("*** Hello!\n");

    auto ide = StrongPtr<Ide>::make(1, 0); // not testing disk latency
    Debug::printf("*** IDE object constructed.\n");
    
    auto fs = StrongPtr<Ext2>::make(ide);
    Debug::printf("*** EXT2 filesystem view object constructed.\n");
    Debug::printf("*** \tblock size is %d\n",fs->get_block_size());
    Debug::printf("*** \tinode size is %d\n",fs->get_inode_size());
    ASSERT(fs->root->number == 2);

    // reduce available memory
    char* trash = (char*) malloc(4 << 20);
    Debug::printf("*** Large memory region allocated.\n");

    char* path = (char*) malloc(256); // relative path from root
    for (uint32_t i = 0; i <= 244; i++) {
        path[i] = 'X';
    }
    path[245] = '/';
    path[246] = '\0';
    Debug::printf("*** Path generated.\n");

    StrongPtr<Node> directory = fs->find(fs->root, path); // finds the directory "XXX..."
    show("XXX...", directory, false);
    ASSERT(fs->find(directory, ".")->number == directory->number);
    ASSERT(fs->find(directory, "..")->number == fs->root->number);

    StrongPtr<Node> symlink = fs->find(directory, "p"); // finds the symlink 'p' itself
    show("p", symlink, false);
    directory = fs->find(directory, "p/"); // resolves 'p' into the directory 'Q'
    show("Q", directory, false);

    path[246] = 'a';
    path[247] = '\0';
    symlink = fs->find(fs->root, path); // finds the symlink 'a' itself
    show("a", symlink, false);

    char* buffer = (char*) malloc(256);
    buffer[254] = '\0';
    buffer[255] = '!';
    symlink->get_symbol(buffer);
    ASSERT(buffer[254] == 'Q'); // final character should now be 'Q'
    ASSERT(buffer[255] == '!'); // do not overwrite this, even with a null terminator
    Debug::printf("*** Symbol confirmed.\n");

    path[247] = '/'; // force symlink resolution
    path[248] = '\0';
    directory = fs->find(fs->root, path); // trace the chain of 16 symlinks
    show("Q", directory, false);
    uint32_t number = fs->find(fs->root, path)->number;

    char* filename = (char*) malloc(4);
    filename[3] = '\0';

    // check for memory leaks in Node construction
    for (uint32_t i = 0; i < 16; i++) {
        for (char fst = 'a'; fst <= 'z'; fst++) {
            filename[0] = fst;
            filename[1] = '\0';
            ASSERT(fs->find(directory, filename) == nullptr); // path is prefix of actual file
            for (char snd = 'a'; snd <= 'z'; snd++) {
                filename[1] = snd;
                filename[2] = '!';
                ASSERT(fs->find(directory, filename) == nullptr); // actual file is prefix of path
                filename[2] = '\0';
                ASSERT((fs->find(directory, filename) != nullptr) == (fst == snd)); // path matches actual filename
            }
            filename[1] = fst;       
        }
    }
    
    Debug::printf("*** Files found.\n");

    // check for memory leaks in find() (also mildly stresses efficiency)
    for (uint32_t i = 0; i < 300; i++) {
        directory = fs->find(fs->root, path);
        ASSERT(directory->number == number);
    }
    Debug::printf("*** Intensive finds completed.\n");

    StrongPtr<Node> file = fs->find(directory, "/message.txt"); // absolute filepath
    show("message.txt", file, true);

    // don't mess with Texas!
    free(path); 
    free(filename);
    free(trash); 
    Debug::printf("*** So long, and thanks for all the fish!\n");
}

