#include "debug.h"
#include "critical.h"
#include "atomic.h"

Atomic<bool> critical_flag { false };
uint32_t critical_depth = 0;
uint32_t critical_owner = 0;

static uint32_t counter = 0;

void work() {
    Debug::printf("*** hello %d\n",counter);
    Debug::printf("*** goodbye %d\n",counter);
    counter ++;
}

/* Called by all cores */
void kernelMain(void) {
    critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();critical([]{work();});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});});
}