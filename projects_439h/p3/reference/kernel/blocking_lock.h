#pragma once

#include "debug.h"
#include "queue.h"
#include "threads.h"
#include "bb.h"

class BlockingLock {
public:
    bool locked;
    BB<bool>* bb = new BB<bool>(1);

    BlockingLock() : locked(false) {}
    BlockingLock(BlockingLock const&) = delete;

    inline void lock() {
        bool a = 1;
        bb->put(a);
    }

    inline void unlock() {
        bb->get();
    }

};