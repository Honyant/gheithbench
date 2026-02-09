#include "atomic.h"
#include "barrier.h"
#include "config.h"
#include "debug.h"
#include "promise.h"
#include "smp.h"
#include "threads.h"
#include <stdint.h>

// this is a pretty simple test. it just checks that you are able to spawn a
// bunch of threads and switch into them, and that you don't lose any threads
// in promises. Along the way it tests a lot of blocking.
constexpr int N = 100;

// for first test
Atomic<int> b{N};
Atomic<int> counter1{0};

// for second test
Atomic<int> counter2{0};

// for third test
Atomic<int> counter3{0};
Promise<uint32_t> promise{};
Barrier b2{N + 1};

void testQueueSize() {

  for (int i = 0; i < N; i++) {
    thread([]() -> void {
      counter1.fetch_add(1);
      b.fetch_add(-1);
    });
  }
}

void recursiveThreadCreation() {
  thread([]() -> void {
    counter2.fetch_add(1);
    if (counter2.get() < N) {
      recursiveThreadCreation();
    }
  });
  while (counter2.get() < N) {
    yield();
  }
}

void hellaPromisesTest() {
  for (int i = 0; i < N; i++) {
    thread([]() -> void {
      counter3.fetch_add(1);
      promise.get();
      counter3.fetch_add(1);
      b2.sync();
    });
  }
}

/* Called by one core */
void kernelMain(void) {

  testQueueSize();

  while (b.get() != 0) {
    yield();
  }
  int counter1val = counter1.get();
  if (counter1val != N) {
    Debug::printf("*** oh no! counter should be %d but is %d.\n", N,
                  counter1val);
    Debug::printf(
        "*** This is likely an issue with your thread pool retrieval.\n");
  } else {
    Debug::printf("*** thread pool test passed.\n");
  }

  recursiveThreadCreation();

  if (counter2.get() < N) {
    Debug::printf("*** error with recursive thread creation");
  } else {
    Debug::printf("*** recursive thread creation test passed!\n");
  }

  hellaPromisesTest();
  while (counter3.get() < N) {
    yield(); // spin
  }
  promise.set(1);
  b2.sync();
  if (counter3.get() != N * 2) {
    Debug::printf("*** promise and barrier test failed.\n");
    Debug::printf("*** This may be an issue with double checking.\n");
  } else {
    Debug::printf("*** promise and barrier test passed!\n");
  }
}
