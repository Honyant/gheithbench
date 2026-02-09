#include "stdint.h"
#include "debug.h"
#include "shared.h"
#include "barrier.h"
#include "atomic.h"
#include "idt.h"
#include "pit.h"

Atomic<uint32_t> counter {0};

/*
Source: ta0 from p3 -- only used to delay sleeping to test efficiency
*/
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

// This test case will test functionality + a few edge cases related to strong/weak ptrs

void kernelMain(void) {

    {
        Debug::printf("*** Test 1: Basic Functionality\n");
        StrongPtr<int> p1 {new int{5}};
        WeakPtr<int> w1 {p1};
        WeakPtr<int> w2 {w1};

        ASSERT(w1.promote() == w2.promote());
        p1 = nullptr;
        ASSERT(w1.promote() == nullptr);
        ASSERT(w2.promote() == nullptr);

        StrongPtr<Atomic<int>> p2 = StrongPtr<Atomic<int>>(new Atomic<int>{10});
        StrongPtr<Atomic<int>> p3 {p2};
        ASSERT(p3 == p2);
        ASSERT(p3->get() == p2->get());
        p3->set(50);
        ASSERT(p2->get() == 50);

        Debug::printf("*** Test 1 Passed\n");
    }

    {
        Debug::printf("*** Test 2: Edge Cases\n");

        // strong ref count != 0 and weak ref count becomes 0
        StrongPtr<Atomic<int>> p1 {new Atomic<int>{15}};
        StrongPtr<Atomic<int>> p2 {p1};
        StrongPtr<Barrier> barrier {new Barrier(2)};

        // should not delete state (ref counts, etc)
        thread([p1, p2, barrier] () mutable {
            WeakPtr<Atomic<int>> w1 {p1};
            barrier->sync();
        });
        barrier->sync();
        ASSERT(p1 == p2);
        ASSERT(p1->get() == 15 && p2->get() == 15);

        // perform some operations to cause things to break if state is deleted prematurely
        WeakPtr<Atomic<int>> w1 {p1};
        w1.promote();
        p1 = nullptr;

        // assignment to itself
        StrongPtr<Atomic<int>> p3 {new Atomic<int>{15}};
        p3 = p3;    // should not delete ptr
        ASSERT(p3->get() == 15);
        WeakPtr<Atomic<int>> w2 {p3};
        p3 = nullptr;
        w2 = w2;    // should not delete state
        ASSERT(w2.promote() == nullptr);

        Debug::printf("*** Test 2 Passed\n");
    }

    {
        Debug::printf("*** Test 3: Application\n");
        uint32_t NUM_THREADS = 100;

        uint32_t debug_trap_vector = 1;
        IDT::interrupt(debug_trap_vector, (uint32_t) trapHandlerAsm);

        set_single_step();

        Debug::printf("*** enabled single-step mode\n");

        // try to test O(1) sleep using shared ptrs
        StrongPtr<Barrier> barrier {new Barrier(NUM_THREADS + 1)};
        for (uint32_t i = 0; i < NUM_THREADS; i++) {
            thread([barrier] () mutable {
                set_single_step();
                sleep(3);   // if it's not O(1) or you add to ready q this will take a while with single step
                barrier->sync();
                counter.fetch_add(1);
                ASSERT((getFlags() & 0x0100) != 0); 
            });
        }

        barrier->sync();
        while (counter < NUM_THREADS) {
            ASSERT((getFlags() & 0x0100) != 0);
        }
        Debug::printf("*** Test 3 Passed\n");
    }    
}

