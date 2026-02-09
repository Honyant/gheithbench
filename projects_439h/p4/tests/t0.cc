#include "debug.h"
#include "threads.h"
#include "atomic.h"

Atomic<uint32_t> thread_step{0};
Atomic<uint32_t> main_step{0};

/* Called by one core */
void kernelMain(void) {
    Debug::printf("*** hello\n");

    const uint32_t n = 5;

    thread([n] {
        for (uint32_t i=0; i<n; i++) {
            while (i != thread_step.get());
            Debug::printf("*** in thread, i=%u\n", i);
            main_step.fetch_add(1);
        }
    });

    for (uint32_t i=1; i<=n; i++) {
        while (i != main_step.get());
        Debug::printf("*** in main, i=%d\n", i);
        thread_step.fetch_add(1);
    }

}

