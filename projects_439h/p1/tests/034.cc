#include "debug.h"
#include "critical.h"
#include "atomic.h"
#include "smp.h"

static uint32_t numIterationsDone = 0;
static uint32_t recursionDepth = 0;
static uint32_t totalIterations = 100;
static int32_t sequenceVals[4];
static Barrier* barrier = new Barrier(4);

/*
This test case will test recursive and various critical sections by making all 4 cores increment/decrement their
own values. Each core's value will follow a unique sequence that will be stored in a static array so that they all compute different numbers.
*/

void incrementSeqVal() {
    if (recursionDepth == 3) {
        return;
    }
    sequenceVals[SMP::me()] += (7 * (SMP::me() + 2));
    recursionDepth++;
    incrementSeqVal();
}
void decrementSeqVal() {
    if (recursionDepth == 3) {
        return;
    }
    sequenceVals[SMP::me()] -= (3 * (SMP::me() + 2));
    recursionDepth++;
    decrementSeqVal();
}

void workNeg()
{
    recursionDepth = 0;
    critical(decrementSeqVal);
}

void workPos()
{
    recursionDepth = 0;
    critical(incrementSeqVal);
}

void printResult() { // ensure only core 0 prints out the result
    if (SMP::me() == 0 && numIterationsDone == totalIterations * 4) {
        for (uint32_t i = 0; i < 4; i++) {
            Debug::printf("*** Core %d computed: %d\n", i, sequenceVals[i]);
        }
    }
}

void incIteration() {
    numIterationsDone++;
}

void actuallyPrintIteration() {
    if (numIterationsDone % 4 == 0) {
        Debug::printf("*** We finished iteration %d\n", numIterationsDone);
    }
}

void printIterationDone() {
    critical(incIteration);
    critical(actuallyPrintIteration);
}

/* Called by all cores */
void kernelMain(void)
{
    sequenceVals[SMP::me()] = (SMP::me() + 2) * 4; // ensures each cores will have its own initial value
    for (uint32_t i = 0; i < totalIterations; i++) {
        if (SMP::me() % 2 == 0) {
            critical(workPos); // will increment the core's current value
        } else {
            critical(workNeg); // will decrement the core's current value
        }
        critical(printIterationDone); // prints every 4th iteration
    }

    barrier->sync(); // ensures all the instructions above have fully ran for all 4 cores
    critical(printResult); // prints all of the cores' final sequence results
}
