#pragma once

#include "ide.h"
#include "shared.h"

// A wrapper around an i-node
class Node : public BlockIO { // we implement BlockIO because we
                              // represent data

public:

    const uint32_t number = 0;

    // How many bytes does this i-node represent
    //    - for a file, the size of the file
    //    - for a directory, implementation dependent
    //    - for a symbolic link, the length of the name
    uint32_t size_in_bytes() override {
        MISSING();
        return 0;
    }

    // read the given block (panics if the block number is not valid)
    // remember that block size is defined by the file system not the device
    void read_block(uint32_t number, char* buffer) override;

    inline uint16_t get_type() {
        MISSING();
        return 0;
    }

    // true if this node is a directory
    bool is_dir() {
        MISSING();
        return false;
    }

    // true if this node is a file
    bool is_file() {
        MISSING();
        return false;
    }

    // true if this node is a symbolic link
    bool is_symlink() {
        MISSING();
        return false;
    }

    // If this node is a symbolic link, fill the buffer with
    // the name the link referes to.
    //
    // Panics if the node is not a symbolic link
    //
    // The buffer needs to be at least as big as the the value
    // returned by size_in_byte()
    void get_symbol(char* buffer);

    // Returns the number of hard links to this node
    uint32_t n_links() {
        MISSING();
        return 0;
    }

    void show(const char* msg) {
    }

    // Returns the number of entries in a directory node
    //
    // Panics if not a directory
    uint32_t entry_count();
};


// This class encapsulates the implementation of the Ext2 file system
class Ext2 {
public:
    StrongPtr<Node> root;
    // Mount an existing file system residing on the given device
    // Panics if the file system is invalid
    Ext2(StrongPtr<Ide> ide);

    // Returns the block size of the file system. Doesn't have
    // to match that of the underlying device
    uint32_t get_block_size() {
        MISSING();
        return 0;
    }

    // Returns the actual size of an i-node. Ext2 specifies that
    // an i-node will have a minimum size of 128B but could have
    // more bytes for extended attributes
    uint32_t get_inode_size() {
        MISSING();
        return 0;
    }

    // If the given node is a directory, return a reference to the
    // node linked to that name in the directory.
    //
    // Returns a null reference if "name" doesn't exist in the directory
    //
    // Panics if "dir" is not a directory
    StrongPtr<Node> find(StrongPtr<Node> dir, const char* name) {
        MISSING();
        return {};
    }

};
