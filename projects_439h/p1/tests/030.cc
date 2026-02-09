#include "critical.h"
#include "debug.h"
#include "random.h"
#include "smp.h"
#include <cstdint>

// this test case tests a lot nesting for calls to critical, which can
// particularly find a bug where critical() uses SMP::me() to check for a
// recursive call without any sort of depth check
#define ARR_SIZE 100

int nums[ARR_SIZE];
int fibTarget;

Random *rng = new Random(5318008);

void genFib() {
  int old = fibTarget;
  if (fibTarget > 1 && nums[fibTarget] == 0 &&
      (nums[fibTarget - 1] == 0 || nums[fibTarget - 2] == 0)) {
    fibTarget = old - 1;
    critical(genFib);
  }
  nums[old] = nums[old - 1] + nums[old - 2];
}

void printNums() {
  for (int i = 0; i < ARR_SIZE; i++) {
    Debug::printf("***fib #%d: %d!\n", i, nums[i]);
  }
}

void randomGen() {
  fibTarget = rng->next() % ARR_SIZE;
  critical(genFib);
}

void forcedMaxFibGen() {
  fibTarget = ARR_SIZE - 1;
  critical(genFib);
}

/* Called by all cores */
void kernelMain(void) {
  for (int repeat = 0; repeat < 15; repeat++) {
    nums[0] = 1;
    nums[1] = 1;
    critical(randomGen);
  }
  critical(forcedMaxFibGen);
  critical(printNums);
}