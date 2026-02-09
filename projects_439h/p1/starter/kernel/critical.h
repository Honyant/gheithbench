#pragma once

#include "atomic.h"
#include "debug.h"

// Implements a critical section, only one core can be in the critical
// section at a time.
//


template <typename Work>
inline void critical(Work work) {
    // Do the work in a critical section

    MISSING();
    //work();
}

