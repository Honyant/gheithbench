#include "atomic.h"
#include "barrier.h"
#include "debug.h"
#include "promise.h"
#include "stdint.h"
#include "threads.h"
#include "pit.h"
#include "idt.h"

static volatile uint32_t counter = 0;

extern "C" void apitHandler(uint32_t* things);

extern "C" void trapHandler(uint32_t* things) {
    uint32_t oldFlags = things[8 + 2]; // Read the old value of eflags from the interrupt frame
    if ((oldFlags & 0x0200) == 0) {
        return; // If normal interrupts are disabled, return
    }

    auto id = SMP::me();
    if (id == 0) {
        Pit::jiffies -= 1; // Prevent the normal handler from double incrementing jiffies
    }

    __atomic_fetch_add(&counter, 1, __ATOMIC_RELAXED);

    // Is the spurious eoi an issue?
    apitHandler(things);
}

extern "C" void trapHandlerAsm();
asm(R"""(
trapHandlerAsm:
    # TODO: handle other registers: XMM, MMX, FP, ...
    pusha             # Push all core registers to the stack
    push %esp         # Push the stack pointer to the stack, as the first argument to trapHandler
    call trapHandler
    pop %esp
    popa              # Restore state
    iret              # Return to interrupted code
)""");

extern "C" void set_single_step();
extern "C" void clear_single_step();
asm(R"""(
set_single_step:
    pushf
    orl $0x0100, 0(%esp)
    popf
    ret
clear_single_step:
    pushf
    andl $0xFFFFFEFF, 0(%esp)
    popf
    ret
)""");

void kernelMain(void) {
    uint32_t debug_trap_vector = 1;
    IDT::interrupt(debug_trap_vector, (uint32_t) trapHandlerAsm);

    set_single_step();

    Debug::printf("*** enabled single-step mode\n");

    Promise<uint32_t>* promise = new Promise<uint32_t>();
    Barrier* barrier = new Barrier(9);

    for (int i = 0; i < 8; i++) {
        thread([promise, barrier] {
            set_single_step();

            uint32_t value = promise->get();
            Debug::printf("*** value: %d\n", value);
            barrier->sync();

            ASSERT((getFlags() & 0x0100) != 0); // Ensure single step mode is still enabled
        });
    }

    uint32_t value = 4217;
    Debug::printf("*** setting value to %d\n", value);
    promise->set(value);
    barrier->sync();
    Debug::printf("*** done\n");
    Debug::printf("| ran in %d instructions\n", counter);

    bool enabled = (getFlags() & 0x0100) != 0; // Ensure single step mode is still enabled
    Debug::printf("*** single step mode: %s\n", enabled ? "enabled" : "disabled");
}

