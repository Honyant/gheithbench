#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "bb.h"

constexpr int N = 2500;

/* This test attempts to test that bb maintains FIFO order
 * even when there are multiple threads attempting to add
 * from the buffer at the same time.
 */
void kernelMain(void) {
    auto bb = new BB<int>(4);

    // Atomic variable to control the turn of the threads
    Atomic<int32_t> turn{0};  

    // Thread turn % 4 == 0
    thread([bb, &turn] {
        for (int i = 0; i < N; i += 4) {
            while (turn.get() % 4 != 0);  
            bb->put(i);
            turn.add_fetch(1);  
        }
    });

    // Thread turn % 4 == 1 
    thread([bb, &turn] {
        for (int i = 1; i < N; i += 4) {
            while (turn.get() % 4 != 1);  
            bb->put(i);
            turn.add_fetch(1);  
        }
    });

    // Thread turn % 4 == 2 
    thread([bb, &turn] {
        for (int i = 2; i < N; i += 4) {
            while (turn.get() % 4 != 2);  
            bb->put(i);
            turn.add_fetch(1);  
        }
    });

    // Thread turn % 4 == 3 
    thread([bb, &turn] {
        for (int i = 3; i < N; i += 4) {
            while (turn.get() % 4 != 3); 
            bb->put(i);
            turn.add_fetch(1);  
        }
    });

    int last = bb->get();
    ASSERT(last == 0);  // The first element should be 0

    while (last < N - 1) {
        auto expected = last + 1;
        last = bb->get();
        if (last != expected) {
            Debug::panic("*** expected: %d last: %d\n", expected, last);
        }
    }

    Debug::printf("*** all good %d\n", last);
}
