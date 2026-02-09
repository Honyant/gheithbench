#include "atomic.h"
#include "barrier.h"
#include "blocking_lock.h"
#include "debug.h"
#include "threads.h"

// i love aribrary numbers
#define NUM_THREADS 100

/*
  Let's help you debug your BlockingLock.

  This case is pretty simple, but your blocking lock will compete a lot.
*/

void kernelMain(void) {
  /*
    TEST ONE

    ONE LOCK COMPETE PER THREAD
   */

  // L annie
  uint32_t pink_bagels = 0;                    // counter
  auto locco = new BlockingLock();             // like rocco from elmo :)
  auto barrier = new Barrier(NUM_THREADS + 1); // barrier for safety

  for (int i = 0; i < NUM_THREADS; ++i) {
    // 100 competes
    thread([&pink_bagels, locco, barrier] {
      locco->lock();
      pink_bagels++; // +1
      locco->unlock();

      // sync bc we done
      barrier->sync();
    });
  }

  // sync here
  barrier->sync();

  Debug::printf("*** GOT: %u PINK BAGELS\n*** EXPECTED: %u PINK BAGELS\n",
                pink_bagels, NUM_THREADS);

  /*
    TEST TWO

    TWO LOCK COMPETES PER THREAD
  */
  uint32_t chrill_bagels = 0; // counter
  barrier = new Barrier(NUM_THREADS + 1);
  auto locked_in = new BlockingLock();
  for (int i = 0; i < NUM_THREADS; ++i) {
    thread([&chrill_bagels, locco, locked_in, barrier] {
      locco->lock();
      locked_in->lock();
      chrill_bagels += 2; // +2
      locked_in->unlock();
      locco->unlock();

      // sync bc we done
      barrier->sync();
    });
  }

  // sync here
  barrier->sync();

  Debug::printf("*** GOT: %u CHRILL BAGELS\n*** EXPECTED: %u CHRILL BAGELS\n",
                chrill_bagels, 2 * NUM_THREADS);

  /*
    TEST THREE

    THREE LOCK COMPETES PER THREAD
  */

  uint32_t sriram_bagels = 0; // counter
  barrier = new Barrier(NUM_THREADS + 1);
  auto lockheed = new BlockingLock();
  for (int i = 0; i < NUM_THREADS; ++i) {
    thread([&sriram_bagels, locco, locked_in, lockheed, barrier] {
      locco->lock();
      locked_in->lock();
      lockheed->lock();
      sriram_bagels += 3; // +3
      lockheed->unlock();
      locked_in->unlock();
      locco->unlock();

      // sync bc we done
      barrier->sync();
    });
  }

  // sync here
  barrier->sync();

  Debug::printf("*** GOT: %u SRIRAM BAGELS\n*** EXPECTED: %u SRIRAM BAGELS\n",
                sriram_bagels, 3 * NUM_THREADS);

  if (pink_bagels == NUM_THREADS && chrill_bagels == 2 * NUM_THREADS &&
      sriram_bagels == 3 * NUM_THREADS)
    Debug::printf("*** よくできました！(trans: well done!)\n*** CELEBRATE YOUR "
                  "NEW 001 PASS WITH SOME BAGELS!\n");
}
