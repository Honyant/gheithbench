#include "atomic.h"
#include "barrier.h"
#include "debug.h"
#include "promise.h"
#include "threads.h"

// This test is meant to help you debug and validate your implementation

// The test is split up into parts that start easy and progressively get more
// difficult.

// Each part will only print one line to the .ok file
// You can comment out tests that you aren't currently debugging to save time.

Barrier barrier1{2};

Atomic<int> n_for_part_2{200};
Atomic<int> count_for_part_2{0};
Barrier barrier2{n_for_part_2 + 1};

Atomic<int> n_for_part_3{50};
Atomic<int> count_for_part_3{0};
Barrier *barrier3 = new Barrier(n_for_part_3 + 2);

void kernelMain(void) {
  // Part 1: A single promise is set

  // This is the most basic functionality of a promise.

  auto promise1 = new Promise<int>();
  thread([promise1] {
    promise1->set(10);
    barrier1.sync();
  });
  yield();
  barrier1.sync();
  int value1 = promise1->get();
  if (value1 == 10) {
    Debug::printf("*** Part 1 SUCCESS: %d\n", value1);
  } else {
    Debug::printf("*** Part 1 FAILURE: %d\n", value1);
  }

  // Part 2: Test stop() deletes and promises are efficient

  // This test will spawn a lot of threads. You have to delete threads in stop()
  // to ensure you don't run out of memory. You can look at the .raw file to see
  // if you are running out of memory. If so, you need to fix your stop(). This
  // test also immediately sets the promise and then calls get() in each thread.
  // This ensures that you don't assume get() will only happen before set(). You
  // should be able to efficiently handle a Promise that has already been
  // fulfilled.

  auto promise2 = new Promise<int>();
  promise2->set(20);
  for (int i = 0; i < n_for_part_2; i++) {
    thread([&promise2, i]() {
      int value = promise2->get();
      if (value == 20) {
        count_for_part_2.fetch_add(1);
      } else {
        Debug::printf("*** Part 2 FAILURE!: %d\n", value);
      }
      barrier2.sync();
    });
  }
  while (count_for_part_2.get() < n_for_part_2.get()) {
    yield();
  }
  barrier2.sync();

  if (count_for_part_2.get() == n_for_part_2.get()) {
    Debug::printf("*** Part 2 SUCCESS: %d\n", promise2->get());
  } else {
    Debug::printf("*** Part 2 FAILURE: %d\n", promise2->get());
  }

  // Part 3: Set is called in the middle of N gets()

  // There will be N/2 gets(), then 1 set, and then N/2 gets()
  // All of the gets() should be value to find the value of the promise.

  // This approximates that set is called in the middle of N gets(), it is not
  // guaranteed. If you use a FIFO queue, set() should be called in the middle.
  // Either way, this should ensure that your set() and get() are agnostic of
  // the order in which they are called.

  auto promise3 = new Promise<int>();
  int N = n_for_part_3.get();

  // First half of get() calls
  for (int i = 0; i < N / 2; i++) {
    thread([&promise3, i]() {
      int value = promise3->get();
      if (value == 30) {
        count_for_part_3.fetch_add(1);
      } else {
        Debug::printf("*** Part 3 FAILURE!: %d\n", value);
      }
      barrier3->sync();
    });
  }

  // Set the promise value
  thread([&promise3]() {
    promise3->set(30);
    barrier3->sync();
  });

  // Second half of get() calls
  for (int i = N / 2; i < N; i++) {
    thread([&promise3, i]() {
      int value = promise3->get();
      if (value == 30) {
        count_for_part_3.fetch_add(1);
      } else {
        Debug::printf("*** Part 3 FAILURE!: %d\n", value);
      }
      barrier3->sync();
    });
  }

  // Wait for all threads to complete
  while (count_for_part_3.get() < N) {
    yield();
  }
  barrier3->sync();

  if (count_for_part_3.get() == N) {
    Debug::printf("*** Part 3 SUCCESS: %d\n", promise3->get());
  } else {
    Debug::printf("*** Part 3 FAILURE: %d\n", promise3->get());
  }
}
