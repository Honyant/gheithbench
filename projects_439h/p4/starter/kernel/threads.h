#pragma once

#include "atomic.h"
#include "queue.h"
#include "heap.h"
#include "debug.h"
#include "smp.h"

constexpr size_t STACK_BYTES = 8 * 1024;
constexpr size_t STACK_WORDS = STACK_BYTES / sizeof(uintptr_t);  /* sizeof(word) == size(void*) */

namespace impl::threads {


};

[[noreturn]]
extern void stop();
extern void yield();
extern void sleep(uint32_t seconds);

template <typename T>
void thread(T const& f) {
    MISSING();
}

