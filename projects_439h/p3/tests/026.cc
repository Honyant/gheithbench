#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"
#include "promise.h"
#include "blocking_lock.h"

int value = 0;

// This is a simple sanity test that verifies your barriers, blocking lock, and sleep work :)
void kernelMain(void) {
    Barrier* barrier;
    BlockingLock* lock = new BlockingLock();
    int count = 0;
    for (int i = 0; i < 3; i++) {
        barrier = new Barrier((i + 1) * 20 + 1);
        Debug::printf("*** Created barrier for %d threads\n", (i + 1) * 20);
        for (int j = 0; j < 20; j++){
            thread([&barrier, &count, &lock] {
                while (true){
                    auto old_barrier = barrier;
                    lock->lock();
                    count++;
                    lock->unlock();
                    old_barrier->sync();
                    while (old_barrier == barrier) {
                        sleep(1);
                    }
                }
            });
        }
        lock->lock();
        count++;
        lock->unlock();
        barrier->sync();
        Debug::printf("*** sync() has been called %d times\n", count);
    }
    Debug::printf("*** all done :)\n");
}
