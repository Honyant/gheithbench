#include "elf.h"
#include "mem.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <cstdint>

void mov(uint32_t inst)
{
    uint8_t sf = b(inst, 31, 1);
    uint16_t hw = b(inst, 21, 2);
    uint64_t immd = b(inst, 5, 16);
    uint8_t rd = b(inst, 0, 5);
    if (DEBUG)
        printf("MOV: sf %d, hw %d, immd %ld, rd %d\n", sf, hw, immd, rd);
    registers[rd] = (immd << (hw*16)) & SFMASK;
}

void add(uint32_t inst)
{

    uint8_t sf = b(inst, 31, 1);
    uint8_t sh = b(inst, 22, 1);
    int64_t imm12 = b(inst, 10, 12);
    if (sh)
        imm12 = imm12 << 12;
    uint8_t rn = b(inst, 5, 5);
    uint8_t rd = b(inst, 0, 5);
    if (DEBUG)
        printf("ADD: sf %d, sh %d, imm12 %ld, rn %d, rd %d\n", sf, sh, imm12, rn, rd);
    uint64_t operand1 = (rn == 31 ? registers[31] : registers[rn]); // TODO & SFMASK;
    uint64_t result = operand1 + imm12;

    if (rd == 31)
        registers[31] = result;
    else
        registers[rd] = result & SFMASK;
}

void cbnz(uint32_t inst)
{
    uint8_t sf = b(inst, 31, 1);
    int64_t imm19 = b(inst, 5, 19);
    uint8_t rt = b(inst, 0, 5);
    if (DEBUG)
        printf("CBNZ: sf %d, imm19 %ld, rt %d\n", sf, imm19, rt);
    uint64_t operand = registers[rt] & SFMASK;
    if (operand)
    {
        imm19 = sign_extend((imm19 << 2), 19);
        pc += imm19 - 4;
    }
}

void adrp(uint32_t inst)
{
    uint64_t immlo = b(inst, 29, 2);
    uint64_t immhi = b(inst, 5, 19);
    uint8_t rd = b(inst, 0, 5);
    if (DEBUG)
        printf("ADRP: immlo %ld, immhi %ld, rd %d\n", immlo, immhi, rd);
    int64_t imm = sign_extend((immhi << (12 + 2)) + (immlo << 12), 33);
    registers[rd] = ((pc & (~0xfff)) + imm);
}

void ldr1(uint32_t inst)
{
    uint8_t sf = b(inst, 30, 1);
    int64_t imm9 = sign_extend(b(inst, 12, 9), 9);
    uint8_t rn = b(inst, 5, 5);
    uint8_t rt = b(inst, 0, 5);
    if (DEBUG)
        printf("LDR1: sf %d, imm9 %ld, rn %d, rt%d \n", sf, imm9, rn, rt);
    uint64_t addr = registers[rn];
    uint64_t data = (sf ? mem_read64(addr) : mem_read32(addr));
    registers[rt] = data & SFMASK;
    addr += imm9;
    registers[rn] = addr;
}

void ldr2(uint32_t inst)
{
    uint8_t sf = b(inst, 30, 1);
    int64_t imm9 = sign_extend(b(inst, 12, 9), 9);
    uint8_t rn = b(inst, 5, 5);
    uint8_t rt = b(inst, 0, 5);
    if (DEBUG)
        printf("LDR2: sf %d, imm9 %ld, rn %d, rt%d \n", sf, imm9, rn, rt);
    uint64_t addr = registers[rn];
    addr += imm9;
    uint64_t data = (sf ? mem_read64(addr) : mem_read32(addr));
    registers[rn] = addr;
    registers[rt] = data & SFMASK;
}

void ldr3(uint32_t inst)
{
    uint8_t sf = b(inst, 30, 1);
    uint64_t imm12 = b(inst, 10, 12);
    uint8_t rn = b(inst, 5, 5);
    uint8_t rt = b(inst, 0, 5);
    if (DEBUG)
        printf("LDR3: sf %d, imm12 %ld, rn %d, rt%d \n", sf, imm12, rn, rt);
    uint64_t addr = registers[rn];
    uint64_t data = (sf ? mem_read64(addr + (imm12 << (sf + 2))) : mem_read32(addr + (imm12 << (sf + 2))));
    registers[rt] = data & SFMASK;
}

void ldrb1(uint32_t inst)
{
    int64_t imm9 = sign_extend(b(inst, 12, 9), 9);
    uint8_t rn = b(inst, 5, 5);
    uint8_t rt = b(inst, 0, 5);
    if (DEBUG)
        printf("LDRB1: imm9 %ld, rn %d, rt%d \n", imm9, rn, rt);
    uint64_t addr = registers[rn];
    uint64_t data = mem_read8(addr);
    registers[rt] = data & 0xfff;
    addr += imm9;
    registers[rn] = addr;
}

void ldrb2(uint32_t inst)
{
    int64_t imm9 = sign_extend(b(inst, 12, 9), 9);
    uint8_t rn = b(inst, 5, 5);
    uint8_t rt = b(inst, 0, 5);
    if (DEBUG)
        printf("LDRB2: imm9 %ld, rn %d, rt%d \n", imm9, rn, rt);
    uint64_t addr = registers[rn];
    addr += imm9;
    uint64_t data = mem_read8(addr);
    registers[rn] = addr;
    registers[rt] = data & 0xfff;
}

void ldrb3(uint32_t inst)
{
    uint64_t imm12 = b(inst, 10, 12);
    uint8_t rn = b(inst, 5, 5);
    uint8_t rt = b(inst, 0, 5);
    if (DEBUG)
        printf("LDRB3: imm12 %ld, rn %d, rt%d \n", imm12, rn, rt);
    uint64_t addr = registers[rn];
    uint64_t data = mem_read8(addr + imm12);
    registers[rn] = addr;
    registers[rt] = data;
}

void orr(uint32_t inst)
{

    uint8_t sf = b(inst, 31, 1);
    uint64_t N = b(inst, 22, 1);
    uint64_t immr = b(inst, 16, 6);
    uint64_t imms = b(inst, 10, 6);
    uint8_t rn = b(inst, 5, 5);
    uint8_t rd = b(inst, 0, 5);
    if (DEBUG)
        printf("ORR: sf %d, N %ld, immr %ld, imms %ld, rn %d, rd %d\n", sf, N, immr, imms, rn, rd);
    if (sf == 0 && N != 0)
    {
        fail = 2;
        return;
    }

    int log2size = 6;
    if (N == 0){
        for (int i = 5; i > 0; i--){//go from right to left to find the 0
            log2size--;
            if (b(imms,i,1) == 0) break;
        }
    }
    uint64_t size = 1 << log2size;
    imms &= ((1 << (log2size)) - 1);
    uint64_t imm = (1ULL << (imms + 1)) - 1;
    uint64_t temp = imm;
    imm = 0;
    for (int i = 0; i < (sf ? 64 : 32); i += (1 << log2size)){
        imm = temp + (imm << ((uint64_t)1 << log2size));
    }
    imm = rotr(imm, ((uint64_t) immr) & ((size==64)?(0ULL-1):((1ULL << size) - 1)), (sf ? 64 : 32));
    uint64_t operand = registers[rn];
    uint64_t result = operand | imm;
    registers[rd] = result & SFMASK;
}

void strb1(uint32_t inst)
{
    int64_t imm9 = sign_extend(b(inst, 12, 9), 9);
    uint8_t rn = b(inst, 5, 5);
    uint8_t rt = b(inst, 0, 5);
    if (DEBUG)
        printf("STRB1: imm9 %ld, rn %d, rt%d \n", imm9, rn, rt);
    uint64_t addr = registers[rn];
    uint8_t data = registers[rt];
    mem_write8(addr, data);
    addr += imm9;
    registers[rn] = addr;
}

void strb2(uint32_t inst)
{
    int64_t imm9 = sign_extend(b(inst, 12, 9), 9);
    uint8_t rn = b(inst, 5, 5);
    uint8_t rt = b(inst, 0, 5);
    if (DEBUG)
        printf("STRB2: imm9 %ld, rn %d, rt%d \n", imm9, rn, rt);
    uint64_t addr = registers[rn];
    uint8_t data = registers[rt];
    addr += imm9;
    mem_write8(addr, data);
    registers[rn] = addr;
}

void strb3(uint32_t inst)
{
    uint64_t imm12 = b(inst, 10, 12);
    uint8_t rn = b(inst, 5, 5);
    uint8_t rt = b(inst, 0, 5);
    if (DEBUG)
        printf("STRB3: imm12 %ld, rn %d, rt%d \n", imm12, rn, rt);
    uint64_t addr = registers[rn];
    uint8_t data = registers[rt];
    mem_write8(addr + imm12, data);
}

//pointers to functions
void (*functions[INSTRUCTION_COUNT])(uint32_t) = {cbnz, adrp, add, ldr1, ldr2, ldr3, ldrb1, ldrb2, ldrb3, mov, orr, strb1, strb2, strb3};

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("usage: %s <ARM executable file name>\n", argv[0]);
        exit(-1);
    }

    uint64_t entry = loadElf(argv[1]);
    pc = entry;
    while (!fail) // fetch, decode, execute
    {
        uint32_t instruction = mem_read32(pc);
        int command = getCommand(instruction);
        if (DEBUG) printf("%lx : ", pc);
        if (DEBUG) printBits(sizeof(instruction), &instruction);
        if (DEBUG) printf(" : ");
        if (command == -1) fail = 2; 
        else if (*functions[command]) (*functions[command])(instruction);
        
        if (fail){
             printf("unknown instruction %08x at %lx\n", instruction, pc);
        }

        pc += 4;
    }
    printRegisters();
    return 0;
}
