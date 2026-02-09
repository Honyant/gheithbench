/*
MCMC to find the mysterious value of pi (v2)!
Adds barrier, promise and random functionality with inspiration from t1.
*/


#include <stdint.h>
#include "smp.h"
#include "debug.h"
#include "config.h"
#include "threads.h"
#include "barrier.h"
#include "random.h"
#include "atomic.h"
#include "promise.h"
constexpr int N = 100000;

/* Called by one core */

Atomic<uint32_t> count = 0;


void kernelMain(void) {
    int nCores = kConfig.totalProcs;
    int nThreads = nCores * 2;
    volatile uint32_t *perCore = new uint32_t[nCores]{};
    volatile uint32_t *perThread = new uint32_t[nThreads]{};

    auto b = new Barrier(nThreads+1);
    auto b2 = new Barrier(nThreads+1);

    auto answer = new Promise<double>();

    for (int i = 0; i<nThreads; i++) {
        thread([i,perCore,perThread,b,b2,answer]() {
            auto r = new Random(i);
            for (int j = 0; j < N; j++) {
                perCore[SMP::me()] += 1;
                perThread[i] += 1;

                auto val1 = r->next();
                auto val2 = r->next();

                double x = (val1 % 10000000 / 10000000.0) * 2-1;
                double y = (val2 % 10000000 / 10000000.0) * 2-1;



                if (x*x+y*y<1)
                    count.fetch_add(1);

                yield();
            }
            b->sync();
            Debug::printf("*** pi = %f\n", answer->get() );
            b2->sync();
        });
    }
    b->sync();
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
    answer->set(4.0*count/(N*nThreads));
    Debug::printf("*** pi = %f\n", answer->get() );
    b2->sync(); //make sure we don't shutdown first
}

