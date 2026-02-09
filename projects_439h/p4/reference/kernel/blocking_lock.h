#pragma once

#include "debug.h"
#include "semaphore.h"

class BlockingLock {
   public:
    Semaphore s{1};
    BlockingLock() {}
    BlockingLock(BlockingLock const&) = delete;
    inline void lock() { s.wait(); }
    inline void unlock() { s.signal(); }
};