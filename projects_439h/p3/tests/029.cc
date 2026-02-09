#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"
#include "bb.h"
#include "blocking_lock.h"

BB<int>* buffer = new BB<int>(50);
Barrier* b = new Barrier(51);
BlockingLock* daLock = new BlockingLock();
int nums[50];
int counter = 0;
Atomic<bool> end{false};

void kernelMain(void) {

    // TEST 1: Ensures that Bounded Buffer preserves order
    for (int i = 0; i < 50; i++) {
        buffer -> put(i);
    }

    for (int i = 0; i < 50; i++) {
        thread([] {
            daLock -> lock();
            nums[counter] = buffer -> get();
            counter++;
            daLock -> unlock();
            b -> sync();
        });
    }

    b -> sync();

    for (int i = 0; i < 50; i++) {
        Debug::printf("*** %d\n", nums[i]);
    }

    // preemption needs to work here
    for (int i = 0; i < 100; i++) {
        thread([] {
            while (true);
        });
    }

    thread([] {
        Debug::printf("*** we made it!\n");
        end.set(true);
    });

    while (!end.get());


}