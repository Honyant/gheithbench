#include "debug.h"
#include "critical.h"
#include "atomic.h"

static uint32_t counter = 0;

void work() {
    Debug::printf("*** hello %d\n",counter);
    Debug::printf("*** goodbye %d\n",counter);
    counter ++;
}

/* Called by all cores */
void kernelMain(void) {
    // The C way, use function pointers
    critical(work);
}
