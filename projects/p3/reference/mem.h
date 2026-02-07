#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <unordered_map>
// This is the memory interface
// We assume that memory is zero-filled
extern uint64_t pc;
struct custom_hash {
    size_t operator()(uint64_t x) const {
        return x ^ (x >> 8);
    }
};

extern std::unordered_map<uint64_t, uint8_t, custom_hash> memory;
// Write the given byte value at the given address
extern void mem_write8(uint64_t address, uint8_t data);

extern uint8_t mem_read8(uint64_t addr);

extern uint64_t mem_read32(uint64_t addr);

extern uint64_t mem_read64(uint64_t addr);

extern int fail;