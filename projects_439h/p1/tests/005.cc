#include "debug.h"
#include "critical.h"
#include "atomic.h"
#include "config.h"

volatile bool do_work[4] {true, true, true, true};
volatile uint32_t factorial_operand = 13;
volatile uint32_t factorial_accum = 1;

void factorial() {
    if (factorial_operand >= 1) {
        Debug::printf("*** Computing factorial of %d\n",factorial_operand);
        factorial_accum *= factorial_operand;
        factorial_operand -= 1;
    } else {
        do_work[SMP::me()] = false;
    }
}

/* Called by all cores */
void kernelMain(void) {
    // The C way, use function pointers
    uint32_t cur_proc = SMP::me();
    while (do_work[cur_proc]) {
        critical(factorial);
    }
    if (cur_proc == 0) {
        Debug::printf("*** Result is: %d\n",factorial_accum);
    }
}
