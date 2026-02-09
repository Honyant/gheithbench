#include "debug.h"
#include "critical.h"
#include "atomic.h"

/**************** Welcome the the MyEEE Test Case! ****************/
// MyEEE stands for My Epic Emulator Emulator (pun on IEEE lol)

// This assignment is an emulator, and its running on our actual 
// computers, so why not put another emulator on this emulator?

// In this case, I'm emulating a processor. There's a main memory 
// array, a small set of registers, some basic functions that 
// represent ARM instructions, and an array of function structs 
// that represent a program.

// Essentially, I want each emulated instruction to run atomically,
// so to do this, I put each instruction in a critical in kernel
// main. However, the twist is that the last core to complete an 
// instruction doesn't get to run the next one. So basically, a 
// different core should be running each instruction.

// main memory
static uint32_t main_memory[4] = { 0x0F, 0x01, 0x03, 0x0 };

// registers
static uint32_t registers[3] = { 0x0, 0x0, 0x0 };

static int num_instructions = 10;

// global flags to ensure only one core runs each instruction and no core does two in arow
static bool done[10] = { false, false, false, false, false, false, false, false, false, false }; // need one per instruction
static uint32_t last_owner = uint32_t(-1);

// Barrier
static Barrier* barriers[10] = { new Barrier(kConfig.totalProcs),
                                new Barrier(kConfig.totalProcs),
                                new Barrier(kConfig.totalProcs),
                                new Barrier(kConfig.totalProcs),
                                new Barrier(kConfig.totalProcs),
                                new Barrier(kConfig.totalProcs),
                                new Barrier(kConfig.totalProcs),
                                new Barrier(kConfig.totalProcs),
                                new Barrier(kConfig.totalProcs),
                                new Barrier(kConfig.totalProcs) };

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

/* Called by all cores */
void kernelMain(void) {

    for ( int i = 0; i < num_instructions; i++) {
        
        struct Instruction& current_instr = instruction_memory[i];

        critical([current_instr, i] {
            // If this core ran the previous instruction, it can't run this one
            if (SMP::me() == last_owner) return;

            // If this is is the first core with the lock, stop other cores from re-running instruction
            if (done[i]) return;
            done[i] = true;

            // Set this core as last lock owner/instruction runner
            last_owner = SMP::me();

            // Run the current instruction
            current_instr.func(current_instr.arg1, current_instr.arg2, current_instr.arg3);
        });

        // Wait for all cores before proceeding to next instruction
        (*barriers[i]).sync();
    }
}
