#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"
#include "promise.h"
#include "bb.h"
#include "shared.h"

/*
 * Test that creates strong and weak pointers for various synchronization techniques to compute the sum of a sequence of numbers.
 * It also tests promotion and reference counting to ensure objects aren't deleted too early
 */

/* Called by one core */
void kernelMain() {
    int32_t barrierSize = 201; // Number of threads calling "sync()" to compute sum
    StrongPtr<Barrier> b1{new Barrier(barrierSize)};
    StrongPtr<Promise<int32_t>> p1{new Promise<int32_t>()};
    StrongPtr<BB<int32_t>> bb{new BB<int32_t>(barrierSize - 1)};

    // ensuring no deletion occurs for the original strong pointers
    {
        StrongPtr<Promise<int32_t>> p2{p1};
        StrongPtr<Barrier> b2{b1};
        StrongPtr<Promise<int32_t>> p3{p1};
        StrongPtr<Barrier> b3{b1};
    }
    StrongPtr<Promise<int32_t>> p2{p1};
    StrongPtr<Barrier> b2{b1};
    StrongPtr<Barrier> b3{b1};

    // Ensuring reference counts are updated properly (no deletion occurs unless all strong pointers are out of scope)
    for (int32_t i = 0; i < barrierSize - 1; i++) {
        thread([b2, bb, i] {
            WeakPtr<BB<int32_t>> weak_bb{bb};
            WeakPtr<Barrier> weak_bar{b2};
            StrongPtr<BB<int32_t>> strongBB = weak_bb.promote();
            int32_t j = i + 1;
            strongBB->put(j);
            (weak_bar.promote())->sync();
        });       
    }
    p2->set(42);
    Debug::printf("*** Strong ptr promise val: %d\n", p1->get()); // ensuring all promise strong pointers point to same object
    b3->sync(); // ensuring all barrier strong pointers point to same object

    // computing sum of all elements in the bounded buffer using weak pointers
    int32_t sum = 0;
    for (int32_t i = 0; i < barrierSize - 1; i++) {
        sum += bb->get(); // bb should not be deleted from before
    }
    Debug::printf("*** Sum of values in bounded buffer: %d\n", sum);

}
