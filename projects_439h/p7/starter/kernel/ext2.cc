#include "ext2.h"
#include "libk.h"
#include "utils.h"

Ext2::Ext2(StrongPtr<Ide> ide_): ide_(ide_), root() {
    using namespace impl::ext2;

    SuperBlock sb;

    ide_->read(1024,sb);
    iNodeSize = sb.inode_size;
    iNodesPerGroup = sb.inodes_per_group;
    numberOfNodes = sb.inodes_count;
    numberOfBlocks = sb.blocks_count;

    //Debug::printf("inodes_count %d\n",sb.inodes_count);
    //Debug::printf("blocks_count %d\n",sb.blocks_count);
    //Debug::printf("blocks_per_group %d\n",sb.blocks_per_group);
    //Debug::printf("inode_size %d\n",sb.inode_size);
    //Debug::printf("block_group_nr %d\n",sb.block_group_nr);
    //Debug::printf("first_inode %d\n",sb.first_inode);

    blockSize = uint32_t(1) << (sb.log_block_size + 10);

    buffer_cache = StrongPtr<BufferCache>::make(ide_, blockSize, 527, 500000 / blockSize);
    
    nGroups = (sb.blocks_count + sb.blocks_per_group - 1) / sb.blocks_per_group;
    //Debug::printf("nGroups = %d\n",nGroups);
    ASSERT(nGroups * sb.blocks_per_group >= sb.blocks_count);

    auto superBlockNumber = 1024 / blockSize;
    //Debug::printf("super block number %d\n",superBlockNumber);

    auto groupTableNumber = superBlockNumber + 1;
    //Debug::printf("group table number %d\n",groupTableNumber);

    auto groupTableSize = sizeof(BlockGroup) * nGroups;
    //Debug::printf("group table size %d\n",groupTableSize);

    auto groupTable = new BlockGroup[nGroups];
    auto cnt = buffer_cache->read_all(groupTableNumber * blockSize, groupTableSize, (char*) groupTable);
    ASSERT(cnt == groupTableSize);

    iNodeTables = new uint32_t[nGroups];

    for (uint32_t i=0; i < nGroups; i++) {
        auto g = &groupTable[i];

        iNodeTables[i] = g->inode_table;

        //Debug::printf("group #%d\n",i);
        //Debug::printf("    block_bitmap %d\n",g->block_bitmap);
        //Debug::printf("    inode_bitmap %d\n",g->inode_bitmap);
        //Debug::printf("    inode_table %d\n",g->inode_table);

    }

    //Debug::printf("========\n");
    //Debug::printf("iNodeSize %d\n",iNodeSize);
    //Debug::printf("nGroups %d\n",nGroups);
    //Debug::printf("iNodesPerGroup %d\n",iNodesPerGroup);
    //Debug::printf("numberOfNodes %d\n",numberOfNodes);
    //Debug::printf("numberOfBlocks %d\n",numberOfBlocks);
    //Debug::printf("blockSize %d\n",blockSize);
    //for (unsigned i = 0; i < nGroups; i++) {
    //    Debug::printf("iNodeTable[%d] %d\n",i,iNodeTables[i]);
    //}

    //root = new Node(ide,2,blockSize);

    root = get_node(2);

    //root->show("root");

    //root->entries([](uint32_t inode, char* name) {
    //    Debug::printf("%d %s\n",inode,name);
    //});



    //println(sb.uuid);
    //println(sb.volume_name);
}

StrongPtr<Node> Ext2::get_node(uint32_t number) {
    if (number == 0) return {};
    ASSERT(number <= numberOfNodes);
    auto index = number - 1;

    auto groupIndex = index / iNodesPerGroup;
    //Debug::printf("groupIndex %d\n",groupIndex);
    ASSERT(groupIndex < nGroups);
    auto indexInGroup = index % iNodesPerGroup;
    auto iTableBase = iNodeTables[groupIndex];
    ASSERT(iTableBase <= numberOfBlocks);
    //Debug::printf("iTableBase %d\n",iTableBase);
    auto nodeOffset = iTableBase * blockSize + indexInGroup * iNodeSize;
    //Debug::printf("nodeOffset %d\n",nodeOffset);

    auto out = StrongPtr<Node>::make(buffer_cache,number,blockSize);
    buffer_cache->read(nodeOffset,out->data);
    return out;
}

static bool is_slash(char ch) {
    return ch == '/';
}

static bool not_slash(char ch) {
    return ch != '/';
}

StrongPtr<Node> Ext2::find(StrongPtr<Node> dir, const char* name) {

    if (K::streq(name, "dir1/dir2")) {
        Debug::printf("here\n");
    }

    using namespace impl::utils;

    auto get_symbol = [] (StrongPtr<Node> sn) {
        ASSERT(sn != nullptr);
        ASSERT(sn->is_symlink());

        auto n = sn->size_in_bytes() + 1;
        auto p = (char*) malloc(n);
        sn->get_symbol(p);
        String symbol{p, n-1};
        free(p); // we can be more efficient by moving ownership to String
        return symbol;
    };

    if (dir == nullptr) {
        return {};
    }
    if (!dir->is_dir()) {
        return {};
    }

    String path{name};

    if (path.length() == 0) {
        return {};
    }

    uint32_t depth = 0;
    constexpr uint32_t LIMIT = 100;

    while (true) {
        if (path.starts_with('/')) {
            path = path.drop_while(is_slash);
            dir = root;
            continue;
        }

        ASSERT(dir != nullptr);
        ASSERT(dir->is_dir());

        if (path.length() == 0) {
            return dir;
        }

        auto component = path.keep_while(not_slash);
        path = path.drop_while(not_slash);
        auto last = path.length() == 0;
        path = path.drop_while(is_slash);

        if (component.length() == 0) {
            ASSERT(last);
            return dir;
        }

        auto inum = dir->entries([component](uint32_t number, const char* nm, size_t len) -> uint32_t {
            if (component.length() != len) return 0;
            for (size_t i=0; i<component.length(); i++) {
                if (component.at(i) != nm[i]) return 0;
            }
            return number;
        });

        if (inum == 0) {
            return {};
        }

        auto child = get_node(inum);
        if (child == nullptr) {
            return {};
        }

        if (last) {
            return child;
        }

        if (child->is_symlink()) {
            if (depth > LIMIT) return {};
            path = get_symbol(child) + "/" + path;
            depth += 1;
            continue;
        }

        depth = 0;

        if (child->is_dir()) {
            dir = child;
            continue;
        }

        return {};
    }

    return dir;
}


////////////// NodeData //////////////

void NodeData::show(const char* what) {
    Debug::printf("%s\n",what);
    Debug::printf("    mode 0x%x\n",mode);
    Debug::printf("    uid %d\n",uid);
    Debug::printf("    gif %d\n",gid);
    Debug::printf("    n_links %d\n",n_links);
    Debug::printf("    n_sectors %d\n",n_sectors);
}

///////////// Node /////////////

void Node::get_symbol(char* buffer) {
    ASSERT(is_symlink());
    auto sz = size_in_bytes();
    if (sz < 60) {
        memcpy(buffer,&data.direct0,sz);
    } else {
        auto cnt = read_all(0,sz,buffer);
        ASSERT(cnt == sz);
    }
}

void Node::read_block(uint32_t lbn, char* buffer) {
    auto refs_per_block = block_size / 4;

    // follow one level of indirection
    auto follow([this, refs_per_block](uint32_t pbn, uint32_t index) -> uint32_t {
        ASSERT(index < refs_per_block);
        if (pbn == 0) return 0;

        uint32_t out;
        buffer_cache->read(pbn * block_size + index * 4, out);
        return out;
    });

    uint32_t pbn = 0;

    if (lbn < 12) {
        uint32_t* direct = &data.direct0;
        pbn = direct[lbn];
    } else {
        lbn -= 12;
        if (lbn < refs_per_block) {
            pbn = follow(data.indirect_1, lbn); 
        } else {
            lbn -= refs_per_block;
            if (lbn < (refs_per_block * refs_per_block)) {
                auto d1 = lbn / refs_per_block;
                auto d0 = lbn % refs_per_block;
                pbn = follow(follow(data.indirect_2, d1), d0);
            } else {
                lbn -= refs_per_block * refs_per_block;
                auto d0 = lbn % refs_per_block;
                auto t = lbn / refs_per_block;
                auto d1 = t % refs_per_block;
                auto d2 = t / refs_per_block;
                ASSERT(d2 < refs_per_block);
                ASSERT((((d2 * refs_per_block + d1) * refs_per_block) + d0) == lbn);
                pbn = follow(follow(follow(data.indirect_3, d2), d1), d0);
            }
        }
    }

    if (pbn == 0) {
        // zero-filled, sparse
        bzero(buffer, block_size);
    } else {
        auto cnt = buffer_cache->read_all(pbn * block_size, block_size,buffer);
        ASSERT(cnt == block_size);
    }
}

uint32_t Node::entry_count() {
    ASSERT(is_dir());
    uint32_t count = 0;
    entries([&count](uint32_t,const char*,uint32_t) -> uint32_t {
        count += 1;
        return 0;
    });
    return count;
}

