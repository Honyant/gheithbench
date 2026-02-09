// This is a bounded buffers test

// You will need blocking lock
// You do NOT need interrupts/sleep

// This is meant to be simple and easy to debug so you can verify your bounded
// buffer implementation

// If you fail any part of the test, the test will stop because of assert
// statements to reduce timeouts.

#include "atomic.h"
#include "bb.h"
#include "debug.h"
#include "threads.h"

/* Global Variables*/

// M is how many producers/consumers we have when we have more than 1
constexpr int M = 6;
// N is how many bagels to produce/consume
constexpr int N = 15;
// The buffer has only 5 slots to make it easy to debug
constexpr int BUFFER_SIZE = 5;

Atomic<int> p1_produced_count{0};
Atomic<int> p1_consumed_count{0};
Atomic<int> p2_produced_count{0};
Atomic<int> p2_consumed_count{0};
Atomic<int> p3_produced_count{0};
Atomic<int> p3_consumed_count{0};
Atomic<int> p4_produced_count{0};
Atomic<int> p4_consumed_count{0};

void kernelMain(void) {
  // Part 1: Single Producer, Single Consumer

  BB<int> *bb1 = new BB<int>(BUFFER_SIZE);

  thread([bb1] {
    for (int j = 0; j < N; j++) {
      int bagel = j;
      bb1->put(bagel);
      p1_produced_count.fetch_add(1);
      // You can uncomment this print statement if it is helpful
      // Debug::printf("*** Producer produced bagel %d, produced_count: %d\n",
      //               bagel, p1_produced_count.get());
    }
  });

  thread([bb1] {
    for (int j = 0; j < N; j++) {
      bb1->get();
      p1_consumed_count.fetch_add(1);
      // You can uncomment this print statement if it is helpful
      // Debug::printf("*** Consumer consumed bagel, consumed_count: %d\n",
      //               p1_consumed_count.get());
    }
  });

  while (p1_produced_count.get() < N || p1_consumed_count.get() < N) {
    yield();
  }
  Debug::printf(
      "*** PART 1: %d bagels were produced and %d bagels were consumed! \n",
      p1_produced_count.get(), p1_consumed_count.get());
  ASSERT(p1_produced_count.get() == N && p1_consumed_count.get() == N);

  // Part 2: Multiple Producers, Single Consumer

  BB<int> *bb2 = new BB<int>(BUFFER_SIZE);

  for (int i = 0; i < M; i++) {
    thread([bb2, i] {
      for (int j = 0; j < N; j++) {
        int bagel = i * N + j;
        bb2->put(bagel);
        p2_produced_count.fetch_add(1);
      }
    });
  }

  thread([bb2] {
    for (int j = 0; j < M * N; j++) {
      bb2->get();
      p2_consumed_count.fetch_add(1);
    }
  });

  while (p2_produced_count.get() < M * N || p2_consumed_count.get() < M * N) {
    yield();
  }
  Debug::printf(
      "*** PART 2: %d bagels were produced and %d bagels were consumed! \n",
      p2_produced_count.get(), p2_consumed_count.get());
  ASSERT(p2_produced_count.get() == M * N && p2_consumed_count.get() == M * N);

  // Part 3: Single Producer, Multiple Consumers

  BB<int> *bb3 = new BB<int>(BUFFER_SIZE);

  thread([bb3] {
    for (int j = 0; j < M * N; j++) {
      int bagel = j;
      bb3->put(bagel);
      p3_produced_count.fetch_add(1);
    }
  });

  for (int i = 0; i < M; i++) {
    thread([bb3, i] {
      for (int j = 0; j < N; j++) {
        bb3->get();
        p3_consumed_count.fetch_add(1);
      }
    });
  }

  while (p3_produced_count.get() < M * N || p3_consumed_count.get() < M * N) {
    yield();
  }
  Debug::printf(
      "*** PART 3: %d bagels were produced and %d bagels were consumed! \n",
      p3_produced_count.get(), p3_consumed_count.get());
  ASSERT(p3_produced_count.get() == M * N && p3_consumed_count.get() == M * N);

  // Part 4: Multiple Producers, Multiple Consumers

  BB<int> *bb4 = new BB<int>(BUFFER_SIZE);

  for (int i = 0; i < M; i++) {
    thread([bb4, i] {
      for (int j = 0; j < N; j++) {
        int bagel = i * N + j;
        bb4->put(bagel);
        p4_produced_count.fetch_add(1);
      }
    });
  }

  for (int i = 0; i < M; i++) {
    thread([bb4] {
      for (int j = 0; j < N; j++) {
        bb4->get();
        p4_consumed_count.fetch_add(1);
      }
    });
  }

  while (p4_produced_count.get() < M * N || p4_consumed_count.get() < M * N) {
    yield();
  }
  Debug::printf(
      "*** PART 4: %d bagels were produced and %d bagels were consumed! \n",
      p4_produced_count.get(), p4_consumed_count.get());
  ASSERT(p4_produced_count.get() == M * N && p4_consumed_count.get() == M * N);
}