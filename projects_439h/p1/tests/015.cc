/*
    This is a Type 2 Monte-Carlo simulation to estimate the value of pi.
    We iterate 100000 times per core for 4 cores = 400000 total points sampled
    From [-1,1] X [-1,1]. We check if these points (x, y) satisfy
    x^2+y^2 < 1.

    This test case tests to make sure we reset critical_owner when depth is 0.
    Otherwise, we will likely not ever have 400000 in our denominator (den).

    In a real-world use case of Monte-Carlo simulation, we would have much more computation to do
    Aside from updating our results (num, den).
    For example, if we were to generate Markov chains, the two lines

    numbers[2*cpuId] = 2*numbers[2*cpuId]-1;
    numbers[2*cpuId+1] = 2*numbers[2*cpuId+1]-1;

    Would turn into a O(len) computation where len is the length of our chain.
    Proportionally, the size of our criticals would be much smaller.
*/


#include "debug.h"
#include "critical.h"
#include "atomic.h"
#include "random.h"
#include "smp.h"
static uint32_t num = 0;
static uint32_t den = 0;

static Random *myRng = new Random(2024);
static const uint32_t iterations = 100000;

static double numbers [8];

void rand() {

    int cpuId = SMP::me();
    
    numbers[2*cpuId] = myRng->next() % 10000 / 10000.;
    numbers[2*cpuId+1] = myRng->next() % 10000 / 10000.;

}

void inc() {

    int cpuId = SMP::me();

    double x = numbers[2*cpuId];
    double y = numbers[2*cpuId+1];
    if (x*x+y*y < 1) {
        num++;
    }
    den++;
}
void checkout() {
    if (den==iterations * 4) {
        Debug::printf("*** pi = %f\n",4.*num/den);
    }
}

/* Called by all cores */
void kernelMain(void) {
    int cpuId = SMP::me();

    for (uint32_t i=0; i<iterations; i++) {
        // The C way, use function pointers
        critical(rand);

        numbers[2*cpuId] = 2*numbers[2*cpuId]-1;
        numbers[2*cpuId+1] = 2*numbers[2*cpuId+1]-1;

        critical(inc);
    }
    critical(checkout);
}