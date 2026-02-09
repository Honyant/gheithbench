#pragma once

#include "debug.h"

class BlockingLock {
public:
    BlockingLock() {
        MISSING();
    }
    BlockingLock(BlockingLock const&) = delete;
    inline void lock() {
        MISSING();
    }
    inline void unlock() {
        MISSING();
    }
};