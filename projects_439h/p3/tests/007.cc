#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "promise.h"
#include "barrier.h"

constexpr uint32_t N = 5;

void kernelMain(void) {
    Debug::printf("*** Starting test\n");

    // Disable interrupts
    Interrupts::disable();

    ASSERT(Interrupts::isDisabled());
    Debug::printf("*** Interrupts disabled\n");

    auto promise = new Promise<int>();
    auto barrier = new Barrier(N + 1);

    for (uint32_t i = 0; i < N; i++) {
        thread([i, promise, barrier] {
            Interrupts::disable();
            ASSERT(Interrupts::isDisabled());
            if (i == 0) promise->set(42);
            ASSERT(Interrupts::isDisabled());
            barrier->sync();
            ASSERT(Interrupts::isDisabled());
        });
    }

    int result = promise->get();
    ASSERT(Interrupts::isDisabled());
    Debug::printf("*** Promise result: %d\n", result);

    barrier->sync();
    ASSERT(Interrupts::isDisabled());

    Debug::printf("*** All threads synchronized\n");

    // Final check
    ASSERT(Interrupts::isDisabled());
    Debug::printf("*** Test completed successfully\n");
}