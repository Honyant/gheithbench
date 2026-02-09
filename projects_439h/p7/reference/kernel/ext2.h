#pragma once

#include "ext_structs.h"
#include "ide.h"
#include "libk.h"
#include "shared.h"
#include "machine.h"

class Ext2;


extern SuperBlock* g_superblock;
extern const int g_cache_capacity;
extern uint32_t* g_block_cache_nums;
extern char** g_block_cache;


// A wrapper around an i-node
class Node : public BlockIO {  // we implement BlockIO because we
                               // represent data

   public:
    INode* inode;
    uint32_t number = 0;
    Ext2* ext2;
    StrongPtr<Ide> ide;
    DirectoryEntry** d_entries;
    uint32_t d_entry_count = -1;
    char* symlink_name;
    // How many bytes does this i-node represent
    //    - for a file, the size of the file
    //    - for a directory, implementation dependent
    //    - for a symbolic link, the length of the name
    uint32_t size_in_bytes() override {
        if (this->is_dir()) {
            return this->inode->i_size;
        } else if (this->is_file()) {
            return this->inode->i_size;
        } else if (this->is_symlink()) {
            return this->inode->i_size;
        }
        return 0;
    }

    Node(uint32_t inum, Ext2* ptr, StrongPtr<Ide> ide);

    virtual ~Node() = default;

    // read the given block (panics if the block number is not valid)
    // remember that block size is defined by the file system not the device
    void read_block(uint32_t number, char* buffer) override;

    inline uint16_t get_type() { return inode->i_mode; }

    // true if this node is a directory
    bool is_dir() { return (inode->i_mode >> 12) == 0x4; }

    // true if this node is a file
    bool is_file() { return (inode->i_mode >> 12) == 0x8; }

    // true if this node is a symbolic link
    bool is_symlink() { return (inode->i_mode >> 12) == 0xA; }

    void get_inode(uint32_t n);

    // If this node is a symbolic link, fill the buffer with
    // the name the link referes to.
    //
    // Panics if the node is not a symbolic link
    //
    // The buffer needs to be at least as big as the the value
    // returned by size_in_byte()
    void get_symbol(char* buffer);

    // Returns the number of hard links to this node
    uint32_t n_links() { return inode->i_links_count; }

    void show(const char* msg) {}

    // Returns the number of entries in a directory node
    //
    // Panics if not a directory
    uint32_t entry_count();
};

class Ext2 {
public:
    StrongPtr<Node> root;
    int sectors_per_block;
    BlockingLock lock{};
    BlockGroupDescriptorTable** bgdt;

    Ext2(StrongPtr<Ide> ide);
    ~Ext2();

    uint32_t get_block_size() { return 1024 << g_superblock->s_log_block_size; }
    uint32_t get_inode_size() { return 128; }

    // If the given node is a directory, return a reference to the
    // node linked to that name in the directory.
    //
    // Returns a null reference if "name" doesn't exist in the directory
    //
    // Panics if "dir" is not a directory

    StrongPtr<Node> findhelper(StrongPtr<Node> dir, const char* name) {
        if (!dir->is_dir()) {
            Debug::panic("findhelper: not a directory");
        }
        while (*name == '/') name++;
        while (*name != '\0') {
            const char* component_end = name;
            while (*component_end != '/' && *component_end != '\0') component_end++;
            if (component_end == name) {
                name++;
                continue;
            }
            int component_length = component_end - name;
            char component[component_length + 1];
            ::memcpy(component, name, component_length);
            component[component_length] = '\0';
            StrongPtr<Node> next = nullptr;
            uint32_t count = dir->entry_count();
            for (uint32_t i = 0; i < count; i++) {
                DirectoryEntry* entry = dir->d_entries[i];
                if (K::streq(entry->name, component)) {
                    next = new Node(entry->inode, this, dir->ide);
                    break;
                }
            }
            if (next == nullptr) {
                return StrongPtr<Node>(nullptr);
            }
            bool is_last_component = (*component_end == '\0');
            while (next->is_symlink()) {
                if (is_last_component && *component_end != '/') {
                    return next;
                }
                char buffer[next->size_in_bytes() + 1];
                next->get_symbol(buffer);
                buffer[next->size_in_bytes()] = '\0';
                StrongPtr<Node> symlink_target;
                if (buffer[0] == '/') {
                    symlink_target = findhelper(root, buffer + 1);
                } else {
                    symlink_target = findhelper(dir, buffer);
                }
                if (symlink_target == nullptr) {
                    return StrongPtr<Node>(nullptr);
                }
                next = symlink_target;
            }
            if (is_last_component) {
                if (*component_end == '/' && !next->is_dir()) {
                    return StrongPtr<Node>(nullptr);
                }
                return next;
            }
            if (!next->is_dir()) {
                return StrongPtr<Node>(nullptr);
            }
            dir = next;
            name = component_end + 1;
            while (*name == '/') name++;
        }
        return dir;
    }

    StrongPtr<Node> find(StrongPtr<Node> dir, const char* name) {
        if (name[0] == '/') {
            return findhelper(root, name + 1);
        } else {
            return findhelper(dir, name);
        }
    }
};