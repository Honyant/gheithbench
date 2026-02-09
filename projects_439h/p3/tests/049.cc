#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "bb.h"
#include "blocking_lock.h"
#include "barrier.h"
#include "promise.h"

/* Called by one core */
void kernelMain(void) {
    
    Atomic<int> counter(11);

    // test preemption
    for (int i = 0; i < 10; i++) {
        thread([&counter]{
            ASSERT(!Interrupts::isDisabled());
            counter.fetch_add(-1);
            while (counter.get() != 0);
        });
    }

    counter.fetch_add(-1);
    while(counter.get() != 0);

    Debug::printf("*** congrats, u preempt\n");

    // a basic barrier and blocking lock test
    BlockingLock* lock = new BlockingLock();
    Barrier* barrier1 = new Barrier(21);
    int count = 0;
    for (int i = 0; i < 20; i++) {
        thread([&lock, &count, &barrier1]{
            lock->lock();
            count++;
            lock->unlock();

            barrier1->sync();
        });
    }

    // test interrupt state
    ASSERT(!Interrupts::isDisabled());
    yield();
    ASSERT(!Interrupts::isDisabled());
    Interrupts::disable();
    yield();
    ASSERT(Interrupts::isDisabled());
    Interrupts::restore(false);
    ASSERT(!Interrupts::isDisabled());

    barrier1->sync();

    Debug::printf("*** the count is %d\n", count);

    BB<int>* buffer = new BB<int>(100);
    Barrier* barrier2 = new Barrier(3);
    thread([&buffer, &barrier2]{
        for (int i = 0; i < 100; i++) {
            int num = i;
            buffer->put(num);
        }
        barrier2->sync();
    });

    thread([&buffer, &barrier2]{
        for (int i = 0; i < 100; i++) {
            Debug::printf("*** %d\n", buffer->get());
        }
        barrier2->sync();
    });
    barrier2->sync();

    Debug::printf("*** ur done\n");
}