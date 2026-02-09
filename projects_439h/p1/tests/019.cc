#include "debug.h"
#include "critical.h"
#include "atomic.h"

Atomic<int> counter(0);

void kernelMain(void);

void work() {
    Atomic<int> local_counter = counter.fetch_add(1);
    if (local_counter >= 3) {
        return;
    }

    Debug::printf("*** Entering recursion level %d\n", local_counter);

    kernelMain();

    Debug::printf("*** Exiting recursion level %d\n", local_counter);
}

/* Called by all cores */
void kernelMain(void) {
    // The C way, use function pointers
    critical(work);
}
