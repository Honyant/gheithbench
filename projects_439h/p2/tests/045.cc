#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "promise.h"
#include "barrier.h"

int N = 20;
static Barrier barrier(N + 2);
// each thread calculates N choose i, i = 0, 1, ..., N

// simple barrier test
void barrier_combo(){
    int *combinations = new int[N + 1];
    for (int i = 0; i <= N; i++)
    {
        uint32_t k = i;
        thread([k, combinations]
               {
                     uint32_t  n = N;
                     uint32_t r = k;
                     uint32_t res = 1;
                     for (uint32_t i = 0; i < r; i++)
                     {
                          res *= (n - i);
                          res /= (i + 1);
                     }
                     combinations[k] = res;
                     barrier.sync(); });
    }
    barrier.sync();
    for (int i = 0; i <= N; i++)
    {
        Debug::printf("*** %d choose %d = %d\n", N, i, combinations[i]);
    }

    // ensure it sums up to 2^N
    uint32_t sum = 0;
    for (int i = 0; i <= N; i++)
    {
        sum += combinations[i];
    }
    Debug::printf("*** Sum of all combinations = %d\n", sum);
}

// same as barrier test, but uses promises to await results from threads
void promise_combo(){
    Promise<int>** combinations = new Promise<int>*[N + 1];
    for (int i = 0; i <= N; i++)
    {
        combinations[i] = new Promise<int>();
    }
    for (int i = 0; i <= N; i++)
    {
        uint32_t k = i;
        thread([k, combinations]
               {
                     uint32_t  n = N;
                     uint32_t r = k;
                     uint32_t res = 1;
                     for (uint32_t i = 0; i < r; i++)
                     {
                          res *= (n - i);
                          res /= (i + 1);
                     }
                     combinations[k]->set(res);
               });
    }
    uint32_t sum = 0;
    for (int i = 0; i <= N; i++)
    {
        sum += combinations[i]->get();
    }
    Debug::printf("*** Sum of all combinations = %d\n", sum);
    for (int i = 0; i <= N; i++)
    {
        delete combinations[i];
    }
    delete[] combinations;
}
void kernelMain(void) {
    barrier_combo();
    promise_combo();
}

