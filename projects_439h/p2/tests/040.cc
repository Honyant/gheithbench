#include "threads.h"
#include "atomic.h"
#include "debug.h"
#include "barrier.h"
#include "promise.h"

// function summary
void kernelMain(void);
uint32_t nestPromises(uint32_t);

void kernelMain(void)
{
    // first, we do a small stress test with lots of threads
    Atomic<int32_t> test1counter{0};
    int32_t N = 10;
    auto test1 = new Barrier(N + 1);
    for (int32_t i = 0; i < N; i++)
    {
        thread([test1, i, N, &test1counter]
               {
            if (i == N/2){
                Debug::printf("*** running test 1\n");
            } else {
                test1counter.fetch_add(1);
            }
            test1->sync(); });
    }
    test1->sync();

    if (test1counter.get() != N - 1)
    {
        Debug::printf("*** test1 failed\n");
    }
    else
    {
        Debug::printf("*** test 1 passed\n");
    }

    Debug::printf("*** running test 2\n");

    // test2 ensures promises are implemented correctly by creating
    // a nested promise dependency tree
    uint32_t failcount = 0;
    uint32_t depth = 5;
    for (uint32_t i = 0; i < 10; i++)
    {
        auto prom = new Promise<uint32_t>();
        thread([i, prom, depth]
               { prom->set(nestPromises(depth)); });
        if (prom->get() != depth)
        {
            failcount += 1;
        }
    }

    if (failcount == 0)
    {
        Debug::printf("*** test 2 passed\n");
    }
    else
    {
        Debug::printf("*** test 2 failed %d times\n", failcount);
    }
}

uint32_t nestPromises(uint32_t depth)
{
    if (depth == 1)
    {
        return 1;
    }
    auto prom = new Promise<uint32_t>();
    thread([depth, prom]
           { prom->set(nestPromises(depth - 1)); });
    if (prom->get() == depth - 1)
    {
        return depth; // it's a good exercise to convince yourself that
                      // a call to nestedPromises will always return depth.
    }
    else
    {
        return 0;
    }
}