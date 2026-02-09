#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "bb.h"
#include "barrier.h"
#include "promise.h"
#include "shared.h"

int N = 5;
int expectedSum = (N * (N + 1)) / 2;  // Sum of integers from 1 to N

/**
 * This is a synchronization methods test. It makes sure that
 * interrupts are left on throughout the whole test. It also
 * test basic pointers. 
 */

void kernelMain(void) {
    Debug::printf("*** Starting Test\n");

    auto bb = new BB<int>(2);
    ASSERT(!Interrupts::isDisabled());
    auto barrier = new Barrier(3);
    ASSERT(!Interrupts::isDisabled());
    auto promise = new Promise<int>();
    ASSERT(!Interrupts::isDisabled());
    auto b2 = new Barrier(1);

    // put ints into bounded buffer
    thread([bb, barrier, promise] {
        uint32_t sum = 0;
        for (int i = 1; i <= N; i++) {
            bb->put(i);
            ASSERT(!Interrupts::isDisabled());
            sum += i;
        }
        promise->set(sum); 
        ASSERT(!Interrupts::isDisabled());
        barrier->sync();
        ASSERT(!Interrupts::isDisabled());
    });

    // gets the ints from the bounded buffer
    thread([bb, barrier] {
        for (int i = 1; i <= N; i++) {
            int val = bb->get();
            ASSERT(!Interrupts::isDisabled());
            ASSERT(val == i);
            Debug::printf("*** retrieved: %d\n", val);
        }
        barrier->sync();
        ASSERT(!Interrupts::isDisabled());
    });

    barrier->sync();
    ASSERT(!Interrupts::isDisabled());

    int sumFromPromise = promise->get();
    ASSERT(!Interrupts::isDisabled());
    ASSERT(sumFromPromise == expectedSum);

    Debug::printf("*** Test 1 done, starting basic pointers test!\n");
    b2->sync();
    ASSERT(!Interrupts::isDisabled());

    StrongPtr<int> p1;
    ASSERT(p1 == nullptr);
    ASSERT(!Interrupts::isDisabled());

    StrongPtr<int> p2 = StrongPtr<int>::make(5);
    ASSERT(!Interrupts::isDisabled());
    ASSERT(!(p2 == nullptr));

    p1 = p2;
    ASSERT(!Interrupts::isDisabled());
    ASSERT(!(p1 == nullptr));

    WeakPtr w1{p1};
    auto s1 = w1.promote();
    ASSERT(!(s1 == nullptr));
    Debug::printf("*** Test done!\n");

}