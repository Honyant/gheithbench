#pragma once

#include <stdint.h>
#include "heap.h"
#include "debug.h"

// A buffer of a given size, intended to be wrapped in smart pointers

namespace impl::ext2 {

struct Buffer {
    const size_t size;
    void* const data;

    Buffer(size_t size) : size(size), data((size == 0) ? nullptr : malloc(size)) {
        ASSERT(data != nullptr);
    }

    ~Buffer() {
        if (data != nullptr) {
            free(data);
        }
    }

};

}