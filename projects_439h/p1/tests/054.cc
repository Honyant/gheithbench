#include "debug.h"
#include "critical.h"
#include "atomic.h"

static uint32_t current_core_count = 0;
static uint32_t depth = 0;

// Recursively prints the current depth and increments it
// Tests that a core can enter the critical section while another core is executing it
void work() {
    if (depth < 5) {
        Debug::printf("*** Core %d is executing iteration %d\n", current_core_count, depth);
        depth++;
        // test closure syntax
        critical([]() {
            work();
        });
        depth--;
        Debug::printf("*** Core %d is has completed iteration %d\n", current_core_count, depth);
    }
}

/* Called by all cores */
void kernelMain(void) {
    critical(work);
}