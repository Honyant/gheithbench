#include "atomic.h"
#include "critical.h"
#include "debug.h"
#include "loop.h"
#include "smp.h"

/* C++ PSA */
// [&var] lets you capture the variable var by reference so you can modify the
// original variable within the closure function

// DO NOT use [=] - this will copy all local variables by value so each closure
// function gets its own copy of each variable. (modifications to variables
// inside the closure do not affect original variables outside the lambda)

// DO NOT use [&] - this will capture all local varialbes by reference, allowing
// the closure function to modify the original variables. this can be dangerous
// and lead to unintended side effects

/* THIS TEST CASE TESTS */
// 1.  The ability to use a critical section to implement a reusable barrier
//      Citation: The reusable barrier implementation is taken from the Little
//                Book of Semaphores
//      Link: https://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf
//      This test implements a synchronization primitive called a barrier
//      through a critical section. Gheith's provided implementation is not
//      reusable - you have to define a new barrier object for each sync point.
//      Here, we have provided a reusable barrier.
// 2. Calling a critical section multiple times in between each barrier
//      This acts as a stress test to ensure the critical section functions
//      correctly. Gheith's provided t0 and t1 cases only enter the critical
//      sections 4 times in total, which is insufficient to thoroughly test the
//      situation where several cores are contesting a critical section.

const int NUM_CORES = 4;

class ReusableBarrier {
  const int n;
  Atomic<int> count;
  Atomic<int> turnstile1;
  Atomic<int> turnstile2;

public:
  ReusableBarrier(int num_threads)
      : n(num_threads), count(0), turnstile1(0), turnstile2(1) {}

  // Turnstiles control the flow of threads. They are gates that can be
  // locked/unlocked. Turnstile1 controls entry, turnstile2 controls the exit.

  // Rules:
  // 1. Only the nth thread can lock or unlock the turnstiles.
  // 2. Before a thread can unlock the first turnstile, it has to close the
  // second and vice versa; therefore it is impossible for one thread to get
  // ahead of the others by more than one turnstile.

  // Conventions:
  // Locked: zero or negative
  // Unlocked: one or more
  //    Initially, turnstile1 is locked and turnstile2 is unlocked
  // Wait: block if locked
  // Signal: increment
  void wait(const char *msg, int iter = 0) {
    // Phase 1: Entry into the critical section
    // Wait for all the threads to arrive
    critical([this, msg, iter]() {
      // If we are the nth thread to arrive, we need to unlock the first
      if (count.add_fetch(1) == n) {
        turnstile2.set(0); // Lock the second turnstile
        turnstile1.set(1); // Unlock the first turnstile
      }
      // Critical section
      if (iter == 0) {
        Debug::printf("*** %s %d\n", msg, count);
      } else {
        Debug::printf("*** %s %d %d\n", msg, iter, count);
      }
    });
    // WAIT
    // Threads wait at turnstile1 until it is unlocked
    // This ensures no thread can proceed until all threads have arrived
    while (turnstile1.get() <= 0) {
      iAmStuckInALoop(false);
    }
    // SIGNAL
    // Once a thread passes through turnstile1, it signals the turnstile to
    // allow the next thread to proceed
    turnstile1.add_fetch(1);

    // Phase 2: Exit from the critical section
    // Wait for all the threads to execute the critical section
    critical([this]() {
      // If we are the nth thread to arrive, we need to unlock the second
      if (count.add_fetch(-1) == 0) {
        turnstile1.set(0); // Lock the first turnstile
        turnstile2.set(1); // Unlock the second turnstile
      }
    });
    // WAIT
    // Threads wait at turnstile2 until it is unlocked
    // This ensures no thread can exit until all threads have arrived
    while (turnstile2.get() <= 0) {
      iAmStuckInALoop(false);
    }
    // SIGNAL
    // Once a thread passes through turnstile2, it signals the turnstile to
    // allow the next thread to proceed
    turnstile2.add_fetch(1);
  }
};

ReusableBarrier barrier(NUM_CORES);

int sum = 0;

void sumArraySegment(int *array, int startIndex) {
  critical([array, startIndex]() -> void {
    for (int i = 0; i < 3; i++) {
      sum += array[startIndex + i];
    }
  });
}

void kernelMain(void) {
  for (int i = 0; i < 10; i++) {
    barrier.wait("TEST LOOP IS BEGINNING FOR ITER", i + 1);
    barrier.wait("We are before the barrier.");
    barrier.wait("We are past the barrier.");
    barrier.wait("TEST LOOP IS ENDING FOR ITER", i + 1);
  }

  int array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
  sumArraySegment(array, SMP::me() * 3);
  barrier.wait("");
  barrier.wait(
      "All cores have finished summing their portion of the array. The sum is",
      sum);
}