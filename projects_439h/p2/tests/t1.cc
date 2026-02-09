#include <stdint.h>
#include "smp.h"
#include "debug.h"
#include "config.h"
#include "threads.h"

constexpr int N = 100000;

/* Called by one core */
void kernelMain(void) {
    int nCores = kConfig.totalProcs;
    int nThreads = nCores * 2;
    volatile uint32_t *perCore = new uint32_t[nCores]{};
    volatile uint32_t *perThread = new uint32_t[nThreads]{};

    Atomic<int> running { nThreads };

    for (int i = 0; i<nThreads; i++) {
        thread([i,perCore,perThread,&running]() {
            for (int j = 0; j < N; j++) {
                perCore[SMP::me()] = perCore[SMP::me()] + 1;
                perThread[i] = perThread[i] + 1;
                yield();
            }
            running.fetch_add(-1);
        });
    }

    while (running.get() != 0) yield();
    for (int i=0; i<nCores; i++) {
        Debug::printf("| core %d did %d\n",i,perCore[i]);
        if (perCore[i] == 0) {
            Debug::printf("*** core %d didn't do any work\n",i);
        }
    }
    for (int i=0; i<nThreads; i++) {
        Debug::printf("| thread %d did %d\n",i,perThread[i]);
        if (perThread[i] != N) {
            Debug::printf("*** thread %d did %d\n",i,perThread[i]);
        }
    }
    Debug::printf("*** done\n");
}

