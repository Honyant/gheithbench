#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "pit.h"

void kernelMain(void) {

    // make sure to make your jiffies variable volatile in pit.h

    thread([] {
        uint32_t start = Pit::jiffies;
        sleep(1);
        uint32_t end = Pit::jiffies;
        if (end - start >= Pit::secondsToJiffies(1)) {
            Debug::printf("*** passed\n");
        } else {
            Debug::printf("*** didn't sleep for long enough 1\n");
        }
    });

    thread([] {
        uint32_t start = Pit::jiffies;
        sleep(2);
        uint32_t end = Pit::jiffies;
        if (end - start >= Pit::secondsToJiffies(2)) {
            Debug::printf("*** passed\n");
        } else {
            Debug::printf("*** didn't sleep for long enough 2\n");
        }
    });

    thread([] {
        uint32_t start = Pit::jiffies;
        sleep(1);
        uint32_t end = Pit::jiffies;
        if (end - start >= Pit::secondsToJiffies(1)) {
            Debug::printf("*** passed\n");
        } else {
            Debug::printf("*** didn't sleep for long enough 3\n");
        }
    });

    sleep(3);
}

