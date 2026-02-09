#pragma once

#include <stdint.h>
#include "debug.h"

class Barrier {
public:
    Barrier(int32_t const n) {
    }
    Barrier(Barrier const&) = delete;

    void sync() {
        MISSING();
    }
        
};

