#include "stdint.h"
#include "debug.h"
#include "shared.h"
#include "threads.h"
#include "barrier.h"

uint32_t counter = 1;

struct intWrapper {
    uint32_t num;

    intWrapper(uint32_t val) {
        num = val;
    }

    ~intWrapper() {
        Debug::printf("*** deleted %d\n", num);
    }
};


void kernelMain(void) {
    // Test 1: Basic functionality
    {
        StrongPtr<intWrapper> strong{new intWrapper(0)};
        WeakPtr<intWrapper> weak{strong};
        {
            StrongPtr<intWrapper> copy = weak.promote();
        }
        strong = nullptr;
        if (weak.promote() == nullptr) 
            Debug::printf("*** yay!\n");
        else
            Debug::printf("*** boo!\n");
    }

    // Test 2: Thread Safety


    for (int i = 0; i < 50; i++) {
        StrongPtr<uint32_t>* original = new StrongPtr<uint32_t>(new uint32_t(i + 1));
        thread([original]() mutable {
            WeakPtr<uint32_t> copy{*original};
            *original = nullptr;
            ASSERT(copy.promote() == nullptr);
        });
    }


}