#include "atomic.h"
#include "debug.h"
#include "threads.h"
#include "pit.h"

constexpr int N1 = 400, M = 5000000, N2 = 1000;

Atomic<int32_t> count = -1;

void kernelMain(void) {
    Debug::printf("*** Test 1: yielding while threads are sleeping\n");

    // Make a bunch of threads that just sleep for a while, then add to the count
    for (int i = 0; i < N1; i++) {
        thread([] {
            sleep(3);
            uint32_t c = count.add_fetch(1);
            if (c == 0) {
                Debug::printf("*** Started counting\n");
                c = count.add_fetch(1);
            }
            if (c == N1) {
                Debug::printf("*** Done counting\n");
            }
        });
    }

    Debug::printf("*** Yielding\n");
    for (int i = 0; i < M; i++) {
        yield(); // Make sure yielding with no other threads active is efficient, even with threads sleeping
    }
    Debug::printf("*** Done yielding\n");

    uint32_t s = 5 - Pit::seconds();
    sleep(s <= 5 ? s : 0); // Sleep until at least second 5
    Debug::printf("*** Count: %d/%d\n", count.get(), N1);



    Debug::printf("*** Test 2: allocating more threads than there is room for\n");

    // Tests that if you don't have room to allocate a stack, you
    // yield until threads finish to free up space
    for (int i = 0; i < N2; i++) {
        thread([] {});
    }

    Debug::printf("*** You passed :D\n");

}
