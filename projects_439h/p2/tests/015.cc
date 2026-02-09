#include <barrier.h>

#include "debug.h"
#include "threads.h"

constexpr int numThreads = 500;

void kernelMain() {
    // checking for proper promise/barrier queue implementation by creating a bunch of barriers that are waiting to resolve,
    // and try spinning between them for a couple cycles

    auto barrier = new Barrier(numThreads + 1);
    auto endBarrier = new Barrier(numThreads + 1);

    // create a thread for each one
    for (int i = 0; i < numThreads; i++) {
        thread([barrier, endBarrier] {
            barrier->sync();
            endBarrier->sync();
        });
    }

    // now, we create a new thread that will resolve all the barriers after cycling for a bit
    thread([barrier] {
        for (int i = 0; i < 10000000; i++) {
            yield();
        }
        barrier->sync();
    });

    endBarrier->sync();

    Debug::printf("*** finished :3\n");
}

