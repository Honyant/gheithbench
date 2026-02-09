#pragma once

#include <stdint.h>

#include "debug.h"
#include "semaphore.h"

class Barrier {
   private:
    uint32_t count;
    uint32_t goal;
    SpinLock s{};
    Semaphore mutex{1};
    Semaphore barrier{0};

   public:
    Barrier(int32_t const n) : count(n) {}
    Barrier(Barrier const&) = delete;

    void sync() {
        mutex.wait();
        count -= 1;
        mutex.signal();
        if (count == 0) barrier.signal();
        barrier.wait();
        barrier.signal();
    }
};
