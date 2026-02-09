#pragma once

#include "stdint.h"
#include "physmem.h"
#include "shared.h"

class Node;

namespace VMM {

    extern uint32_t global_pd;

    struct VME {
        uint32_t start;
        uint32_t size;
        bool shared;
        StrongPtr<Node> node;
        uint32_t offset;
        uint8_t flags; // WARNING: last bit doesnt actually do anything
        bool user_unmappable;
        VMM::VME* next;
    };

    struct FreeRegion {
        uint32_t start;
        uint32_t size;
        FreeRegion* next;
        FreeRegion* prev;
    };
    extern VME* shared_VMEs;


    extern void init_free_regions();
    
    // Called (on the initial core) to initialize data structures, etc
    extern void global_init();

    // Called on each core to do per-core initialization
    extern void per_core_init();

    void* mmap(uint32_t sz_, bool shared, StrongPtr<Node> node, uint8_t flags, uint32_t offset_, void* addr);

    // naive mmap
    extern void* naive_mmap(uint32_t size, bool shared, StrongPtr<Node> file, uint8_t flags, uint32_t file_offset);

    // naive munmap
    void naive_munmap(void* p);

    int naive_munmap2(void* p);


    // Will map virtual address va to physical
    void pd_map(uint32_t* pd, uint32_t va, uint32_t pa);

    // Will unmap virtual address va
    void pd_unmap(uint32_t* pd, uint32_t va);

}

