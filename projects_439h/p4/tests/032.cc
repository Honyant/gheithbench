/*
Build on top of ta0 by adding a second counter for instructions ran with interrupts disabled.
And add Shared Pointers for the promise and barrier used.
Not necessarily a hard case to pass, but uses the original motivation behind p4:
Allows comparing number disabled vs. enabled as well as deleting of sync primitives like promise and barrier.
Finally, check some simple cases for shared pointers.
*/





#include "atomic.h"
#include "barrier.h"
#include "debug.h"
#include "promise.h"
#include "stdint.h"
#include "threads.h"
#include "pit.h"
#include "idt.h"
#include "shared.h"

static volatile uint32_t counter = 0;
static volatile uint32_t dounter = 0;


extern "C" void apitHandler(uint32_t* things);

extern "C" void trapHandler(uint32_t* things) {
    uint32_t oldFlags = things[8 + 2]; // Read the old value of eflags from the interrupt frame
    if ((oldFlags & 0x0200) == 0) {
        __atomic_fetch_add(&dounter, 1, __ATOMIC_RELAXED);
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

template <typename T>
void check(StrongPtr<T> p, bool expectedNull) {
    Debug::printf("*** %s\n", ((p == nullptr) == expectedNull) ? "happy" : "sad");
}



template <typename T>
void eq(StrongPtr<T> p1, StrongPtr<T> p2) {
    Debug::printf("*** %s\n", (p1 == p2) ? "happy" : "sad");
}

template <typename T>
void ne(StrongPtr<T> p1, StrongPtr<T> p2) {
    Debug::printf("*** %s\n", (p1 != p2) ? "happy" : "sad");
}

struct TestObject {
    int value;

    TestObject(int val) : value(val) {
        Debug::printf("*** TestObject(%d) constructed\n", value);
    }

    ~TestObject() {
        Debug::printf("*** TestObject(%d) destructed\n", value);
    }
};


void kernelMain(void) {
    uint32_t debug_trap_vector = 1;
    IDT::interrupt(debug_trap_vector, (uint32_t) trapHandlerAsm);

    set_single_step();

    Debug::printf("*** enabled single-step mode\n");

    // Use StrongPtr for Promise and Barrier
    StrongPtr<Promise<uint32_t>> promise = StrongPtr<Promise<uint32_t>>::make();
    StrongPtr<Barrier> barrier = StrongPtr<Barrier>::make(9);



    for (int i = 0; i < 8; i++) {
        auto thread_promise = promise; // Capture StrongPtr
        auto thread_barrier = barrier; // Capture StrongPtr
        thread([thread_promise, thread_barrier]() mutable  {

            

            set_single_step();

            uint32_t value = thread_promise->get();
            Debug::printf("*** value: %d\n", value);
            thread_barrier->sync();

            ASSERT((getFlags() & 0x0100) != 0); // Ensure single step mode is still enabled

            // StrongPtr references will be released when going out of scope

            
        });
    }

    uint32_t value = 4217;
    Debug::printf("*** setting value to %d\n", value);
    promise->set(value);
    barrier->sync();
    Debug::printf("*** done\n");
    Debug::printf("| Interrupts enabled for %d instructions\n", counter);
    Debug::printf("| Interrupts disabled for %d instructions\n", dounter);


    bool enabled = (getFlags() & 0x0100) != 0; // Ensure single step mode is still enabled
    Debug::printf("*** single step mode: %s\n", enabled ? "enabled" : "disabled");

    // Release the StrongPtr references in the main thread
    promise = nullptr;
    barrier = nullptr;

    // Use the check function to verify if the pointers are nullptr
    check(promise, true); // Should print "happy"
    check(barrier, true); // Should print "happy"

    clear_single_step();

    //Simple testing shared pointers using just one thread

    Debug::printf("*** Test 1: Destructor call upon scope exit\n");
    {
        StrongPtr<TestObject> obj { new TestObject(1) };
        check(obj, false);
    }

    // Test 2: Self-assignment
    {
        Debug::printf("*** Test 2: Self-assignment\n");
        StrongPtr<TestObject> p { new TestObject(4) };
        p = p;
        check(p, false);
        p = nullptr;
        check(p, true);

    }

}
