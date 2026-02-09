#pragma GCC optimize("O0")

#include "stdint.h"
#include "debug.h"
#include "shared.h"

// Tests freeing all memory associated with smart pointers

struct LargeStruct {
    char large[1000000];
};

uint32_t i = 0;
const uint32_t num_iterations = 1000;

void kernelMain(void) {
    Debug::printf("*** start\n");
    while (i < num_iterations) {
        StrongPtr<LargeStruct> p{new LargeStruct};
        WeakPtr<LargeStruct> w{p};
        const uint32_t j = i + 1;
        i = j;
    }
    Debug::printf("*** finish\n");
}

