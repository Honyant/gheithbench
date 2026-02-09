#include "ext2.h"
#include "libk.h"
#include "ext_structs.h"


// Global variables
SuperBlock* g_superblock = nullptr;
const int g_cache_capacity = 64;
uint32_t* g_block_cache_nums = nullptr;
char** g_block_cache = nullptr;

Ext2::Ext2(StrongPtr<Ide> ide) {
    char* buffer = new char[512];
    ide->read_block(2, buffer);
    g_superblock = new SuperBlock();
    memcpy(g_superblock, buffer, sizeof(SuperBlock));
    this->sectors_per_block = (1024 << g_superblock->s_log_block_size) / 1024;
    int BGDT_start_block = this->get_block_size() == 1024 ? 2 : 1;
    int num_block_groups = g_superblock->s_inodes_count / g_superblock->s_inodes_per_group;
    // Debug::printf("num blocks: %d\n", num_block_groups);
    this->bgdt = (BlockGroupDescriptorTable**) malloc(sizeof(BlockGroupDescriptorTable*) * num_block_groups);
    char* buffer2 = new char[num_block_groups * sizeof(BlockGroupDescriptorTable)];
    ide->read_all(BGDT_start_block * this->get_block_size(), num_block_groups * sizeof(BlockGroupDescriptorTable), buffer2);
    for(int i = 0; i < num_block_groups; i++){
        this->bgdt[i] = (BlockGroupDescriptorTable*)(buffer2 + i * sizeof(BlockGroupDescriptorTable));
    }
    root = StrongPtr<Node>::make(2, this, ide);

    // Initialize global cache
    g_block_cache_nums = new uint32_t[g_cache_capacity]();
    g_block_cache = new char*[g_cache_capacity]();
}

Ext2::~Ext2() {
    // delete g_superblock;
    // int num_block_groups = g_superblock->s_inodes_count / g_superblock->s_inodes_per_group;
    // for (int i = 0; i < num_block_groups; i++) {
    //     delete bgdt[i];
    // }
    // free(bgdt);

    // for (int i = 0; i < g_cache_capacity; i++) {
    //     if (g_block_cache[i] != nullptr)
    //     delete[] g_block_cache[i];
    //     g_block_cache[i] = nullptr;
    // }
    // delete[] g_block_cache;
    // delete[] g_block_cache_nums;
}

///////////// Node /////////////


void Node::get_inode(uint32_t n){
    uint32_t group_num = (n-1)/g_superblock->s_inodes_per_group;
    uint32_t table_block = ext2->bgdt[group_num]->bg_inode_table;
    uint32_t table_relative = (n-1) % g_superblock->s_inodes_per_group;
    uint32_t offset = table_block * ext2->get_block_size() + table_relative * sizeof(INode);
    char* buffer = new char[sizeof(INode)];
    ide->read(offset, sizeof(INode), buffer);
    this->inode = (INode*)buffer;
}

Node::Node(uint32_t inum, Ext2* ext2, StrongPtr<Ide> ide): BlockIO(ext2->get_block_size()), number(inum), ext2(ext2), ide(ide){
    this->get_inode(inum);

    if(this->inode->i_size == 0 ) return;

    if(this->is_symlink()){
        this->symlink_name = new char[this->inode->i_size+1];
        if(this->inode->i_size<60){
            ::memcpy(this->symlink_name, this->inode->i_block, this->inode->i_size);
        }else{
            uint32_t num_blocks = (this->inode->i_size - 1) /  ext2->get_block_size() + 1;
            char* data = new char[(ext2->get_block_size()) * num_blocks];
            for(uint32_t i=0;i<num_blocks;i++){
                read_block(i,data+i*ext2->get_block_size());
            }
            ::memcpy(this->symlink_name, data, this->inode->i_size);
        }
        this->symlink_name[this->inode->i_size] = 0;
        return;
    }

    uint32_t num_blocks = (this->inode->i_size - 1) /  ext2->get_block_size() + 1;

    if(this->is_dir()){
        // Debug::printf("num blocks %d\n", num_blocks);
        char* data = new char[(ext2->get_block_size()) * num_blocks];
        
        for(uint32_t i=0;i<num_blocks;i++){
            read_block(i,data+i*ext2->get_block_size());
        }
        uint32_t entry_count = 0;
        uint32_t offset = 0;
        while(offset<this->inode->i_size){
            DirectoryEntry* entry = (DirectoryEntry*)(data + offset);
            offset += entry->rec_len;
            entry_count += entry->inode != 0;
        }
        this->d_entry_count = entry_count;

        this->d_entries = (DirectoryEntry**) malloc(sizeof(DirectoryEntry*) * entry_count);
        offset = 0;
        entry_count = 0;
        while(offset < this->inode->i_size){
            DirectoryEntry* entry = (DirectoryEntry*)(data + offset);
            DirectoryEntry* real_entry = new DirectoryEntry();
            ::memcpy(real_entry, data + offset, sizeof(DirectoryEntry));
            real_entry->name[real_entry->name_len] = 0; // null terminate
            offset += entry->rec_len;
            this->d_entries[entry_count] = real_entry;
            entry_count += entry->inode != 0;
        }
        // Debug::printf("entry count%d\n", this->d_entry_count);
        delete[] data;    
    }
    
}

uint32_t Node::entry_count() { 
    if(!this->is_dir()) Debug::panic("not a dir");
    return this->d_entry_count; 
}

void Node::get_symbol(char* buffer) { 
    if (!is_symlink()) {
        PANIC("not a symlink");
    }
    memcpy(buffer, this->symlink_name, this->size_in_bytes());
    return;
}

void memset(char* buffer, uint32_t n, char val){
    for(uint32_t i=0;i<n;i++){
        buffer[i] = val;
    }
}

void read_logical_block(Node* node, uint32_t blocknum, char* buffer){
    Ext2* ext2 = node->ext2;
    if(blocknum == 0) {
        memset(buffer, ext2->get_block_size(), 0);
        return;
    }
    ext2->lock.lock();
    uint32_t blocksize = ext2->get_block_size();
    uint32_t hash = blocknum % g_cache_capacity;
    if(g_block_cache_nums[hash] == blocknum){
        ::memcpy(buffer, g_block_cache[hash], blocksize);
    }else{
        node->ide->read_all((blocknum) * blocksize, blocksize, buffer);
        if(g_block_cache[hash] == nullptr){
            g_block_cache[hash] = new char[blocksize];
        }
        ::memcpy(g_block_cache[hash], buffer, blocksize);
        g_block_cache_nums[hash] = blocknum;
    }
    ext2->lock.unlock();
}



void Node::read_block(uint32_t index, char* buffer) {
    uint32_t max_blocks = (this->inode->i_size - 1) / this->ext2->get_block_size() + 1;
    if(index >= max_blocks) Debug::panic("block num %d is not valid", index);
    uint32_t BIPB = ext2->get_block_size() / 4; // block_idxs_per_block
    char* temp_buffer = new char[ext2->get_block_size()];
	// Debug::printf("%d ", index);
    if(index<12){
		uint32_t block_id = this->inode->i_block[index];
        if(block_id==0) {
            memset(buffer, ext2->get_block_size(), 0);
            return;
        }
        read_logical_block(this, block_id, buffer);
    } else if(index < BIPB+12){
        uint32_t top_block_id = this->inode->i_block[12];
        if(top_block_id==0) {
            memset(buffer, ext2->get_block_size(), 0);
            return;
        }
        read_logical_block(this, top_block_id, temp_buffer);
        read_logical_block(this, ((uint32_t*) temp_buffer)[index-12], buffer);
    } else if(index < BIPB*BIPB + BIPB + 12){
        uint32_t top_block_id = this->inode->i_block[13];
        if(top_block_id==0) {
            memset(buffer, ext2->get_block_size(), 0);
            return;
        }
        uint32_t new_index = (index-(BIPB + 12));
        read_logical_block(this, top_block_id, temp_buffer);
        read_logical_block(this, ((uint32_t*) temp_buffer)[new_index / BIPB], temp_buffer);
        read_logical_block(this, ((uint32_t*) temp_buffer)[new_index % BIPB], buffer);
    } else{
		// Debug::printf("sdf");
        uint32_t top_block_id = this->inode->i_block[14];
        if(top_block_id==0) {
            memset(buffer, ext2->get_block_size(), 0);
            return;
        }
        uint32_t new_index = index - (BIPB * BIPB + BIPB + 12);
        read_logical_block(this, top_block_id, temp_buffer);
        read_logical_block(this, ((uint32_t*) temp_buffer) [new_index / BIPB / BIPB], temp_buffer);
        read_logical_block(this, ((uint32_t*) temp_buffer) [new_index / BIPB % BIPB], temp_buffer);
        read_logical_block(this, ((uint32_t*) temp_buffer) [new_index % BIPB], buffer);
    }
    delete[] temp_buffer;

}
