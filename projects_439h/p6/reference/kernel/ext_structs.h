#pragma once

#include <stdint.h>

struct SuperBlock {
    // Total number of inodes in the filesystem, which track file and directory metadata.
    uint32_t s_inodes_count;         

    // Total number of blocks in the filesystem, including all used, free, and reserved blocks.
    uint32_t s_blocks_count;         

    // Number of blocks reserved for the super user to ensure management access.
    uint32_t s_r_blocks_count;       

    // Total number of free blocks currently available in the filesystem.
    uint32_t s_free_blocks_count;    

    // Total number of free inodes available for creating new files or directories.
    uint32_t s_free_inodes_count;    

    // Block number of the first data block, typically 1 for 1KiB block size filesystems.
    uint32_t s_first_data_block;     

    // Log base 2 of the block size, used to calculate actual block size (1024 << s_log_block_size).
    uint32_t s_log_block_size;       

    // Log base 2 of the fragment size, applicable if fragmentation is supported.
    uint32_t s_log_frag_size;        

    // Number of blocks contained in each block group for organization within the filesystem.
    uint32_t s_blocks_per_group;     

    // Number of fragments per block group, defining layout for fragmented data.
    uint32_t s_frags_per_group;      

    // Number of inodes allocated per block group, limiting maximum files per group.
    uint32_t s_inodes_per_group;     

    // Last mount time of the filesystem in Unix time format.
    uint32_t s_mtime;                

    // Last write time to the filesystem in Unix time format.
    uint32_t s_wtime;                

    // Count of how many times the filesystem has been mounted.
    uint16_t s_mnt_count;            

    // Maximum number of mounts allowed before requiring a filesystem check.
    uint16_t s_max_mnt_count;        

    // Magic number identifying the filesystem type, fixed to EXT2_SUPER_MAGIC.
    uint16_t s_magic;                

    // Current state of the filesystem (valid or error).
    uint16_t s_state;                

    // Action taken upon error detection (continue, remount read-only, or panic).
    uint16_t s_errors;               

    // Minor revision level of the filesystem, indicating specific changes or features.
    uint16_t s_minor_rev_level;      

    // Last time the filesystem was checked for integrity, in Unix time format.
    uint32_t s_lastcheck;            

    // Maximum interval in seconds allowed between filesystem checks.
    uint32_t s_checkinterval;        

    // Identifier for the OS that created the filesystem (e.g., Linux, HURD).
    uint32_t s_creator_os;           

    // Revision level of the filesystem, indicating feature set and changes.
    uint32_t s_rev_level;            

    // Default user ID for reserved blocks, typically the superuser.
    uint16_t s_def_resuid;           

    // Default group ID for reserved blocks, typically the superuser group.
    uint16_t s_def_resgid;           

    // Index of the first non-reserved inode available for general use.
    uint32_t s_first_ino;            

    // Size of each inode structure in bytes, which must be a power of 2.
    uint16_t s_inode_size;           

    // Block group number that hosts this superblock for recovery purposes.
    uint16_t s_block_group_nr;       

    // Bitmask indicating compatible filesystem features.
    uint32_t s_feature_compat;       

    // Bitmask indicating incompatible filesystem features; mounting is refused if unsupported.
    uint32_t s_feature_incompat;     

    // Bitmask for read-only compatible features; mount as read-only if unsupported.
    uint32_t s_feature_ro_compat;    
};
struct BlockGroupDescriptorTable {
    // Block number of the first block of the block bitmap for the group.
    uint32_t bg_block_bitmap;         

    // Block number of the first block of the inode bitmap for the group.
    uint32_t bg_inode_bitmap;         

    // Block number of the first block of the inode table for the group.
    uint32_t bg_inode_table;          

    // Total number of free blocks in the block group.
    uint16_t bg_free_blocks_count;     

    // Total number of free inodes in the block group.
    uint16_t bg_free_inodes_count;     

    // Number of inodes allocated to directories in the block group.
    uint16_t bg_used_dirs_count;       

    // Padding to align the structure to a 32-bit boundary.
    uint16_t bg_pad;                  

    // Reserved space for future use.
    uint8_t bg_reserved[12];          
};

struct INode {
    // File mode, indicating the type of file and the access permissions.
    uint16_t i_mode;

    // User ID of the file owner.
    uint16_t i_uid;

    // Size of the file in bytes.
    uint32_t i_size;

    // Last access time.
    uint32_t i_atime;

    // Creation time.
    uint32_t i_ctime;

    // Last modification time.
    uint32_t i_mtime;

    // Deletion time.
    uint32_t i_dtime;

    // Group ID of the file owner.
    uint16_t i_gid;

    // Count of hard links to this inode.
    uint16_t i_links_count;

    // Number of 512-byte blocks allocated to the file.
    uint32_t i_blocks;

    // File flags.
    uint32_t i_flags;

    // OS-specific field.
    uint32_t i_osd1;

    // Array of block pointers.
    uint32_t i_block[15];

    // File version (for NFS).
    uint32_t i_generation;

    // File Access Control List.
    uint32_t i_file_acl;

    // Directory Access Control List (only for directories).
    uint32_t i_dir_acl;

    // Fragment address.
    uint32_t i_faddr;

    // OS-specific field.
    uint8_t i_osd2[12];
};

struct DirectoryEntry {
    // 32 bit inode number of the file entry. A value of 0 indicates that the entry is not used.
    uint32_t inode;
    
    // 16 bit unsigned displacement to the next directory entry from the start of the current directory entry.
    uint16_t rec_len;

    // 8 bit unsigned value indicating how many bytes of character data are contained in the name.
    uint8_t name_len;

    // 8 bit unsigned value used to indicate file type.
    uint8_t file_type;

    // 0-255 bytes of character data representing the name of the file.
    char name[256];
};