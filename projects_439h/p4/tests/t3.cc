#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"


constexpr uint32_t N = 10;
constexpr uint32_t expected = (N * (N+1))/2;

Atomic<uint32_t> count = 0;


/* Called by one core */
void kernelMain(void) {
    Debug::printf("*** hello\n");

    auto b = new Barrier(N+1);

    for (uint32_t i=1; i<=N; i++) {
        thread([i, b] {
            count.fetch_add(i);
            b->sync();
        });
    }

    b->sync();

    auto v = count.get();

    if (v == expected) {
        Debug::printf("*** all good\n");
    } else {
        Debug::printf("*** %d != %d\n", v, expected);
    }


}

