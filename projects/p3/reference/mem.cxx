#include "mem.h"
#include <cstdio>

void mem_write8(uint64_t addr, uint8_t data) {
    if (addr == 0xffffffffffffffff) {
        printf("%c", data);
    }
    memory[addr] = data;
}

uint8_t mem_read8(uint64_t addr) {
    return memory[addr];

}

uint64_t mem_read32(uint64_t addr) {
    return ((uint64_t) memory[addr+3]<<24) + ((uint64_t) memory[addr+2]<<16) + ((uint64_t) memory[addr+1]<<8) + (uint64_t) memory[addr];

}

uint64_t mem_read64(uint64_t addr) {
    return ((uint64_t) memory[addr+7]<<56) + ((uint64_t) memory[addr+6]<<48) + ((uint64_t) memory[addr+5]<<40) + ((uint64_t) memory[addr+4]<<32) + ((uint64_t) memory[addr+3]<<24) + ((uint64_t) memory[addr+2]<<16) + ((uint64_t) memory[addr+1]<<8) + (uint64_t) memory[addr];
}