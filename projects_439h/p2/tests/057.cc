#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "promise.h"
#include "barrier.h"

/**************** Welcome the the MyEEE Test Case 2.0! ****************/
// MyEEE stands for My Epic Emulator Emulator (pun on IEEE lol)

// This assignment is an emulator, and its running on our actual 
// computers, so why not put another emulator on this emulator?

// In this case, I'm emulating a processor. There's a main memory 
// array, a small set of registers, some basic functions that 
// represent ARM instructions, and an array of function structs 
// that represent a program.

// This time, the instructions still happen in order, but each thread
// is the child of the last thread that ran the instruction. In 
// addition, each thread has to wait for all threads to a barrier
// and for the inner most thread to set a promise before moving forward.

// main memory
static uint32_t main_memory[4] = { 0x0F, 0x01, 0x03, 0x0 };

// registers
static uint32_t registers[3] = { 0x0, 0x0, 0x0 };

static uint32_t num_instructions = 10;

// global flags to ensure only one core runs each instruction and no core does two in arow
// static bool done[10] = { false, false, false, false, false, false, false, false, false, false }; // need one per instruction
// static uint32_t last_owner = uint32_t(-1);

// Barriers
static Barrier* barriers[10] = { new Barrier(2),
                                new Barrier(3),
                                new Barrier(4),
                                new Barrier(5),
                                new Barrier(6),
                                new Barrier(7),
                                new Barrier(8),
                                new Barrier(9),
                                new Barrier(10),
                                new Barrier(11) };

// Promises
static Promise<uint32_t> *promises[10] = { new Promise<uint32_t>(),
                                new Promise<uint32_t>(),
                                new Promise<uint32_t>(),
                                new Promise<uint32_t>(),
                                new Promise<uint32_t>(),
                                new Promise<uint32_t>(),
                                new Promise<uint32_t>(),
                                new Promise<uint32_t>(),
                                new Promise<uint32_t>(),
                                new Promise<uint32_t>() };

// load
static void load(uint32_t register1, uint32_t address, uint32_t scratch) {
    registers[register1] = main_memory[address];
}

// store
static void store(uint32_t register1, uint32_t address, uint32_t scratch) {
    main_memory[address] = registers[register1];
}

// add
static void add(uint32_t register1, uint32_t register2, uint32_t val) {
    registers[register1] = registers[register2] + val;
}

// subtract
static void subtract(uint32_t register1, uint32_t register2, uint32_t val) {
    registers[register1] = registers[register2] - val;
}

static void printresult(uint32_t register1, uint32_t register2, uint32_t register3) {
    Debug::printf("*** register 1: %d\n", registers[register1]);
    Debug::printf("*** register 2: %d\n", registers[register2]);
    Debug::printf("*** register 3: %d\n", registers[register3]);
}

// The program being run (set of instructions)
static struct Instruction {
    void (*func)(uint32_t, uint32_t, uint32_t);
    uint32_t arg1;               // First argument
    uint32_t arg2;               // Second argument
    uint32_t arg3;               // Third argument
} instruction_memory[10] = {(struct Instruction){load, 0, 0, 0},
                            (struct Instruction){add, 1, 0, 16},
                            (struct Instruction){store, 1, 0, 0},
                            (struct Instruction){load, 1, 1, 0},
                            (struct Instruction){store, 1, 1, 1},
                            (struct Instruction){load, 2, 2, 0},
                            (struct Instruction){subtract, 2, 2, 3},
                            (struct Instruction){subtract, 2, 0, 8},
                            (struct Instruction){load, 0, 3, 0},
                            (struct Instruction){printresult, 0, 1, 2}};

void recursiveInstructionCompletion(uint32_t n) {
    thread([n] {
        Debug::printf("*** started instruction thread %d.\n", n + 1);
        struct Instruction ins = instruction_memory[n];
        ins.func(ins.arg1, ins.arg2, ins.arg3);
        Debug::printf("*** ended instruction thread %d.\n", n + 1);
        if (n < 9) recursiveInstructionCompletion(n + 1);
        promises[n]->set(n);
        
        for (uint32_t i = n; i < num_instructions; i++)
        {
            barriers[i]->sync();
            promises[i]->get();
        }
    });
}

/* Called by one core */
void kernelMain(void) {
    Debug::printf("*** started kernel main\n");

    recursiveInstructionCompletion(0);

    for (uint32_t i = 0; i < num_instructions; i++)
    {
        barriers[i]->sync();
        promises[i]->get();
    }

}

