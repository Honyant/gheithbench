#pragma once

#include <cstdint>
#include <cstdio>
#include <unordered_map>

#define INSTRUCTION_COUNT 14

#define b(x, y, z) get_bits_at_offset(x, y, z)
#define SFMASK (sf ? 0xffffffffffffffff : 0x00000000ffffffff)

std::unordered_map<uint64_t, uint8_t, custom_hash> memory;
uint64_t registers[32];
uint64_t pc;
int fail = 0;
bool DEBUG = false;//prints out the instructions and instruction addresses if true

uint32_t masks[INSTRUCTION_COUNT] = {
    0b01111111000000000000000000000000,
    0b10011111000000000000000000000000,
    0b01111111100000000000000000000000,
    0b10111111111000000000110000000000,
    0b10111111111000000000110000000000,
    0b10111111110000000000000000000000,
    0b11111111111000000000110000000000,
    0b11111111111000000000110000000000,
    0b11111111110000000000000000000000,
    0b01111111100000000000000000000000,
    0b01111111100000000000000000000000,
    0b11111111111000000000110000000000,
    0b11111111111000000000110000000000,
    0b11111111110000000000000000000000};

uint32_t opcodes[INSTRUCTION_COUNT] = {
    0b00110101000000000000000000000000,
    0b10010000000000000000000000000000,
    0b00010001000000000000000000000000,
    0b10111000010000000000010000000000,
    0b10111000010000000000110000000000,
    0b10111001010000000000000000000000,
    0b00111000010000000000010000000000,
    0b00111000010000000000110000000000,
    0b00111001010000000000000000000000,
    0b01010010100000000000000000000000,
    0b00110010000000000000000000000000,
    0b00111000000000000000010000000000,
    0b00111000000000000000110000000000,
    0b00111001000000000000000000000000};


static inline uint64_t get_bits_at_offset(uint32_t inst, uint64_t offset, uint64_t bits)
{ // gets bits number of bits from offset offset so i can get the data acoording to the arm manual
    return (inst >> offset) & ((1 << (bits)) - 1);
}

void printRegisters()
{
    for (int i = 0; i < 31; i++)
    {
        printf("X%02d : %016lX\n", i, registers[i]);
    }
    printf("XSP : %016lX\n", registers[31]);
}
static inline int64_t sign_extend(uint64_t value, int bits)
{ // does a sign extend based on the number of original bits
    return (int64_t)(value << (64 - bits)) >> (64 - bits);
}

void printBits(size_t const size, void const *const ptr)
{
    unsigned char *b = (unsigned char *)ptr;
    unsigned char byte;
    int i, j;

    for (i = size - 1; i >= 0; i--)
    {
        for (j = 7; j >= 0; j--)
        {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
} // from https://stackoverflow.com/questions/111928/is-there-a-printf-converter-to-print-in-binary-format

uint32_t getCommand(uint32_t instruction)
{
    for (int i = 0; i < INSTRUCTION_COUNT; i++)
    {
        if ((instruction & masks[i]) == opcodes[i])
        {
            return i;
        }
    }
    return -1;
}


uint64_t rotr(uint64_t x, uint64_t n, uint64_t d) {
    return (x >> n) | (x << (d-n));
} //from https://stackoverflow.com/questions/28303232/rotate-right-using-bit-operation-in-c
