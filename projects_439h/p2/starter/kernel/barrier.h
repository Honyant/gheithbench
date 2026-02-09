#pragma once

#include "atomic.h"
#include "threads.h"

class Barrier {
    // Bad implementation, replace it with a blocking one
    Atomic<int32_t> n;
public:
    Barrier(int32_t const n): n(n) {}
    Barrier(Barrier const&) = delete;

    void sync() {
        n.add_fetch(-1);
        while (n.get() > 0) yield();
    }
        
};

