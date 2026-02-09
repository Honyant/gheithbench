#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"
#include "pit.h"

/*
    This is designed to be a simple cache test ensuring you performing caching for ALL disk reads 
    whenever possible.
*/

int ITER = 2500;

void kernelMain(void) {
    auto ide = StrongPtr<Ide>::make(1, 100);
    auto fs = StrongPtr<Ext2>::make(ide);
    auto root = fs->root;

    /* 
        Test 1: caching in find()
    */
    StrongPtr<Node> n = fs->find(root, "dir1/dir2/dir3/dir4/dir5/msg.txt");
    for (int i = 0; i < ITER; i++) {
        // now this should be FAST
        n = fs->find(root, "dir1/dir2/dir3/dir4/dir5/msg.txt");
    }
    
    Debug::printf("*** Test 1: Caching in find() successful!\n");

    /* 
        Test 2: caching in node->read_block() 
    */
    char* buffer = new char[n->size_in_bytes()];
    // n = dir1/dir2/dir3/dir4/dir5/msg.txt
    n->read_all(0, n->size_in_bytes(), buffer);
    for (int i = 0; i < ITER; i++) {
        // now this should be FAST
        n->read_all(0, n->size_in_bytes(), buffer);
    }
    Debug::printf("*** Test 2: Caching in node->read_block() successful!\n");
    free(buffer);
    
    /* 
        Test 3: caching in node->get_symbol() for small links
        simply storing the inode in memory is sufficient
    */
    n = fs->find(root, "linkToMsg");
    buffer = new char[n->size_in_bytes()];
    n->get_symbol(buffer);

    for (int i = 0; i < ITER; i++) {
        // now this should be FAST
        n->get_symbol(buffer);
    }
    Debug::printf("*** Test 3: Caching in node->get_symbol() for small links successful!\n");
    free(buffer);

    /* 
        Test 4: caching in node->get_symbol() for looong links
        if you cached for node->read_block() this probably works automatically
    */
    n = fs->find(root, "long_link_to_msg");
    buffer = new char[n->size_in_bytes()];
    n->get_symbol(buffer);
    for (int i = 0; i < ITER; i++) {
        // now this should be FAST
        n->get_symbol(buffer);
    }
    Debug::printf("*** Test 4: Caching in node->get_symbol() for long links (>60 char) successful!\n");
    free(buffer);
}


