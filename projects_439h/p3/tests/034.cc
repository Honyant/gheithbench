#include "atomic.h"
#include "debug.h"
#include "stdint.h"
#include "threads.h"
#include "barrier.h"
#include "blocking_lock.h"
#include "pit.h"

Barrier begin { 4 };
Barrier end { 4 };

void kernelMain(void) {
    Debug::printf("*** Starting\n");

    for (int i = 0; i < 4; i++) {
        thread([] {
            // Make sure blocking lock doesn't disable interrupts
            BlockingLock l {};
            l.lock();
            ASSERT(!Interrupts::isDisabled());

            Debug::printf("*** Starting to spin\n");
            begin.sync();

            // Tests that jiffies properly increments when yielding in a loop
            // Make sure to make jiffies volatile in pit.h and pit.cc!
            uint32_t target = Pit::secondsToJiffies(4);
            while (Pit::jiffies < target) {
                yield();
            }

            Debug::printf("*** Done\n");
            end.sync();
            Debug::shutdown();
        });
    }
    stop();
}

