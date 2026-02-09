#include "debug.h"
#include "critical.h"
#include "smp.h"

/**
 * This test case tests two things: 
 *  1) Your code supports a method being run 
 *      in the critical secton calling kernelMain
 *    --X FAILURE: 'Iteration ?' is wrong compared to OK
 *    --? FIX: Reconsider exchange logic
 *  2) If critical is called within a critical section,
 *      the core in the critical section is preserved
 *    --X FAILURE: 'Core order not preserved' will print 
 *    --? FIX: Reconsider exchange logic
 */

volatile int kernelMainCalledCounter = 0;
volatile uint32_t currentCore = -1;

void kernelMain(void);

void runner() {
  if(kernelMainCalledCounter == 0) {
    currentCore = SMP::me();
  } else if(kernelMainCalledCounter >= 10) {
    currentCore = -1;
    return;
  }

  if(currentCore != SMP::me()) {
    Debug::printf("*** Core order not preserved. Expected core %d, got %d!\n", currentCore, SMP::me());
    Debug::shutdown();
  }

  kernelMainCalledCounter = kernelMainCalledCounter + 1;
  Debug::printf("*** Iteration %d\n", kernelMainCalledCounter);
  kernelMain();
}

void kernelMain(void) {
  critical(runner);
}
