#include <stdint.h>
#include "smp.h"
#include "threads.h"
#include "bb.h"
#include "atomic.h"
#include "random.h"
#include "shared.h"
#include "barrier.h"

struct Dummy {
    int counter;

    Dummy(int val) {
        counter = val;
    }
};

void kernelMain(void) {

    // Testing whether the reference counting keeps data alive
    SpinLock lock1{};
    StrongPtr<Dummy> d{new Dummy{0}};
    StrongPtr<Barrier> b{new Barrier{101}};
    for (int i = 0; i < 100; i ++) {
        thread([d, b, i, &lock1] () mutable {
            lock1.lock();
            d->counter ++;
            lock1.unlock();
            if (d->counter == 100) Debug::printf("*** Counter reached 100\n");
            d = nullptr;
            b->sync();
            if (i == 0) Debug::printf("*** Printing Post Barrier Sync\n");
        });
    }

    d = nullptr;
    b->sync();
    b = nullptr;

    // Testing whether reference counting is correctly locked
    SpinLock lock{};
    StrongPtr<Dummy> d1{new Dummy{0}};
    for (int i = 0; i < 100; i ++) {
        thread([d1, &lock] {
            for (int i = 0; i < 1000; i ++) {
                StrongPtr<Dummy> d_copy{d1};
                lock.lock();
                d_copy->counter ++;
                lock.unlock();
            }
        });
    }
    
    while (d1->counter < 100000) {
        yield();
    }
    Debug::printf("*** Passed Test Case!\n");
}

