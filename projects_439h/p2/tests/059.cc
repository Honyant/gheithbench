#include "atomic.h"
#include "barrier.h"
#include "debug.h"
#include "promise.h"
#include "stdint.h"
#include "threads.h"

constexpr uint32_t N = 439;
Atomic<uint32_t> begin{N};
Atomic<uint32_t> end{N};

constexpr uint32_t DELAY = 1000000;

constexpr uint32_t VALUE = 439;
Promise<uint32_t> p;
Barrier b{N + 1};

/* Called by one core */
void kernelMain(void) {
  Debug::printf("*** Starting...\n");

  p.set(VALUE);

  Debug::printf("*** Promise is set\n");

  for (uint32_t i = 0; i < N; ++i) {
    thread([] {
      for (uint32_t i = 0; i < DELAY; ++i) {
        ASSERT(p.get() == VALUE);
      }
      b.sync();
    });
  }

  Debug::printf("*** All threads spawned\n");

  b.sync();

  Debug::printf("*** All threads finished\n");
}
