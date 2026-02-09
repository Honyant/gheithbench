#pragma once

#include "stdint.h"
#include "shared.h"
#include "physmem.h"

class Node;

namespace VMM {

    // Called (on the initial core) to initialize data structures, etc
    extern void global_init();

    // Called on each core to do per-core initialization
    extern void per_core_init();

    // naive mmap
    extern void* naive_mmap(uint32_t size, bool shared, StrongPtr<Node> file, uint32_t file_offset);

    // naive munmap
    void naive_munmap(void* p);

}

