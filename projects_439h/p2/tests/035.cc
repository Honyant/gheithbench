#include <stdint.h>
#include "smp.h"
#include "debug.h"
#include "config.h"
#include "threads.h"
#include "promise.h"
#include "barrier.h"

constexpr int N = 100;
uint32_t nCores = kConfig.totalProcs;
Atomic<uint32_t> running {nCores};
Atomic<int> counter {0};

/* Called by one core */
void kernelMain(void) {
    
    /*
    TEST 1: Make sure no threads are randomly lost during yield or stop
    */
    
    for (uint32_t i=0; i<nCores; i++) {
        thread([] {
            for (uint32_t i=0; i<N * 10; i++) {
                yield();
            }
            running.fetch_add(-1);
            stop();
        });
    }

    while (running.get() != 0) yield();
    Debug::printf("*** All threads have stopped\n");


    /*
    TEST 2: Make sure stop threads are properly stopped and freed
    */
    running.set(N);
    for (uint32_t i=0; i<N; i++) {
        thread([] {
            stop();
        });
        running.fetch_add(-1);
    }

    while (running.get() != 0) yield();
    Debug::printf("*** All threads have been freed\n");


    /*
    TEST 3: Valid Promise implementation
    */
    for (uint32_t ans = 0; ans < N; ans++) {
        auto answer = new Promise<int>();
        counter.set(0);
        running.set(N);
        for (uint32_t i=0; i<N; i++) {
            thread([ans, i, answer] {
                if (i == N / 2){
                    answer->set(ans);
                }
                else {
                    auto a = answer->get();
                    if ((i + 1) % 100 == 0) Debug::printf("*** Answer: %d\n", a);
                    counter.fetch_add(1);
                }
                running.fetch_add(-1);
                stop();
            });
        }

        while (running.get() != 0) yield();
    }

    Debug::printf("*** threads ran: %d\n", counter.get());
    Debug::printf("*** Promises added after isReady is set don't disappear + checks for race conditions\n");


    /*
    TEST 4: Valid Barrier implementation
    */
    for (uint32_t i=0; i<N; i++) {
        auto mainBarrier = new Barrier(N + 1);
        auto barrier = new Barrier(N);
        counter.set(0);
        running.set(N);
        for (uint32_t j=0; j<N; j++) {
            thread([i, j, mainBarrier, barrier] {
                barrier->sync();
                counter.fetch_add(1);
                mainBarrier->sync();
                stop();
            });
        }

        mainBarrier->sync();
    }
    Debug::printf("*** threads ran: %d\n", counter.get());
    Debug::printf("*** Barriers added after n = 0 don't disappear + checks for race conditions\n");
}

