#pragma once

#include "atomic.h"
#include "debug.h"
#include "smp.h"
// Implements a critical section, only one core can be in the critical
// section at a time.
//
extern Atomic<bool> c_flag;
extern volatile uint32_t c_depth;
extern volatile uint32_t c_owner;

static SpinLock spinLock;

template <typename Work>
inline void critical(Work work) {
    spinLock.lock();
    if (c_flag.get() && c_owner == SMP::me()) { // we already own the critical section
        c_depth += 1;
    } else { // wait until we can enter the critical section
        while (c_flag.exchange(true)) {
            spinLock.unlock();
            iAmStuckInALoop(true);
            spinLock.lock();
        }
        // we've entered the critical section
        c_owner = SMP::me();
        c_depth = 1;
    }
    spinLock.unlock();
    work();
    spinLock.lock();
    if (c_depth == 1) { // we're exiting the outermost critical section
        c_owner = 0;
        c_flag.set(false);
    }
    c_depth -= 1;
    spinLock.unlock();
}

