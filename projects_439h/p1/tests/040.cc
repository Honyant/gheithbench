#include "debug.h"
#include "critical.h"
#include "atomic.h"
#include "smp.h"

static uint32_t curr = 8;
static uint32_t num = 8;
static uint32_t ans = 1;

void work() {
    // recursively calculate a factorial by using critical, effectively testing recursion in critical
    if (curr != 0) {
        Debug::printf("*** Current value of factorial: %d\n", ans);
        ans = ans * curr;
        curr = curr - 1;
        critical(work);
    } if (curr == 0) {
        curr = num + 1;
        num = num + 1;
        ans = 1;
    }
}

/* Called by all cores */
void kernelMain(void) {
    critical(work);
}
