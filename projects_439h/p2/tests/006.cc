#include "atomic.h"
#include "barrier.h"
#include "config.h"
#include "debug.h"
#include "promise.h"
#include "threads.h"

/*
  THIS IS PRIMARILY BARRIER WAIT QUEUE TEST

  It will ensure that if you use a separate waiting queue in the barrier
  synchronization mechanism, you do so *correctly*. This test is designed
  such that there are many opportunities for threads to be caught in the
  wrong queue. If this happens, they will never run again and you will time
  out.

  If you don't use separate waiting queues, you also probably won't pass.

  Also worth noting, this test case used to have two parts. There was a
  suspected race condition in the first part I didn't wanna mess with. I gave it
  up in case someone else could solve the problem (implementing Barriers with
  Promises). Good luck fellow testcase opps o7. May you implement it well.
*/

// this is arbitrary asf lol
#define ITERS 100
#define FETCHERS 3 // aka lies or something

/*
  BARRIER PROMISE

  Implements Promise with your Barrier.

  This works by maintaining a Barrier with n=FETCHERS + 1. It's not
  exactly semantically the same, but it's close enough for testing purposes.
  `set()` and `get()` both call to `sync()` the barrier. The return
  value is set immediately upon calling `set()` but it's unavailable in
  `get()` until both are called.

  Don't try and actually use this code. You'll get caught. It's performance
  is also terrible.
*/
class BarrierPromise {
private:
  int value;
  int value2;
  Atomic<bool> done;
  Barrier barrier;
  SpinLock lock;
  SpinLock dwayne_the_lock;

public:
  BarrierPromise() : value(0), value2(0), done(false), barrier(FETCHERS + 1) {}

  // bro... :skull:
  void set(int const &v) {
    ASSERT(!done.get());

    value = v;      // set value
    barrier.sync(); // wait til we all get here
                    // we wait after we set so that
                    // we dont prematurely return wrong value

    lock.lock(); // wow this code sucks
    dwayne_the_lock.lock();
    if (!value2) {
      dwayne_the_lock.unlock();
      done.set(true); // future get() are ok
    }
    lock.unlock();
  }

  int get() {
    // sorry for this, it's terrible
    // youll have to convince urself this makes sense :(
    dwayne_the_lock.lock();
    lock.lock(); // lock for condition check
    if (!done.get()) {
      dwayne_the_lock.unlock(); // unlock so sync() works
      lock.unlock();

      // wheeeeeeeeeeeeeeeeeeeeeee
      barrier.sync();

      lock.lock(); // lock for condition check
      dwayne_the_lock.lock();
    }
    dwayne_the_lock.unlock();
    lock.unlock(); // unlock after condition pass
    return value;
  }
};

/* Called by one core */
void kernelMain(void) {
  /*
    A loop will iterate ITERS times.

    Each time, it creates a single thread to set the value of a BarrierPromise,
    and FETHCERS threads to get said value, print it out, and sync for the
    iteration.

    The whole thing syncs as well at the end, to make sure nothing was lost.
  */

  Debug::printf("\n*** BARRIER PROMISE INIT\n*** ");

  // ensure all iterations finish before test ends
  // keep these barriers on the heap for safety :)
  auto done_barrier = new Barrier(ITERS * FETCHERS + 1);
  auto fr_done_barrier = new Barrier(ITERS * FETCHERS + 1);

  for (int i = 0; i < ITERS; ++i) {
    // heapy!
    auto iter_barrier = new Barrier(FETCHERS + 1);
    auto promise = new BarrierPromise();

    // spawn a bunch of recieving threads
    for (int j = 0; j < FETCHERS; ++j) {
      // these resolve second
      thread([promise, iter_barrier, done_barrier, fr_done_barrier] {
        // get val
        int val = promise->get();
        Debug::printf("%d", val); // print 2027

        // sync
        iter_barrier->sync();
        done_barrier->sync();
        fr_done_barrier->sync(); // make sure you dont get stuck
      });
    }

    // this resolves first
    thread([promise] {
      // let them know we are here + send
      Debug::printf(" :3 "); // :3
      promise->set(2027);    // set promise
    });

    iter_barrier->sync(); // make sure promises finishe in iter
                          // thanks cooper! :)))
  }

  done_barrier->sync();
  fr_done_barrier->sync(); // do you get stuck?

  // you're done!
  Debug::printf(
      "\n*** よくできました！(translation: \"well done!\")\n"
      "*** Redeem a free bagel at Einstein's by showing your brand new 006 "
      "pass.\n");
}
