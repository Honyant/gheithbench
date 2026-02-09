#include "debug.h"
#include "critical.h"

static uint32_t counter = 0;
static uint32_t exponent = 5;

/* Called by all cores */
void kernelMain(void) {
    // The C++ way, use closures (C++ and Python call them lambdas)
    critical([] {
        Debug::printf("*** hello %d\n",counter);
        const int MAX = 1 << exponent;
        int l = 0, r = MAX;
        int target = 13;
        int steps = 0;
        while (l < r) {
            steps++;
            int mid = (l + r) >> 1;
            Debug::printf("*** mid %d\n", mid);
            if (mid < target) l = mid+1;
            else r = mid;
        }

        Debug::printf("*** took %d steps. goodbye %d\n\n", steps, counter);
	    counter++;
        exponent += 5;
    });
}
