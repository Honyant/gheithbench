#include "barrier.h"
#include "config.h"
#include "debug.h"
#include "smp.h"
#include "threads.h"
#include <stdint.h>

// this test is an adaptation of t0 that tests for multiple queue sizes.
// this is particularly meant to test the race condition caused by yielding with
// few items in the queue
constexpr int N = 10000;

void testQueueSize(int nThreads) {
  int nCores = kConfig.totalProcs;
  volatile uint32_t *perCore = new uint32_t[nCores]{};
  volatile uint32_t *perThread = new uint32_t[nThreads]{};

  Atomic<int> running{nThreads};

  for (int i = 0; i < nThreads; i++) {
    thread([i, perCore, perThread, &running]() {
      for (int j = 0; j < N; j++) {
        perCore[SMP::me()] = perCore[SMP::me()] + 1;
        perThread[i] = perThread[i] + 1; // only one thread accesses this index
        yield();
      }
      running.fetch_add(-1);
    });
  }

  while (running.get() != 0)
    yield();

  for (int i = 0; i < nThreads; i++) {
    Debug::printf("*** did %d on thread %d\n", perThread[i], i);
  }
}

/* Called by one core */
void kernelMain(void) {

  for (int i = 1; i < 20; i++) {
    testQueueSize(i);
  }
  Debug::printf("*** done\n");
}
