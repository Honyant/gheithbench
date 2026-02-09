#include "debug.h"
#include "threads.h"
#include "barrier.h"
#include "bb.h"


/* Called by one core */
void kernelMain(void) {
    uint32_t num_threads = 150;
    // this will create num_threads threads on sleep(1)
    //                  num_threads threads on sleep(3)
    //                  num_threads threads on sleep(5)

    // This testcase will stress test sleep, if you timeout check that your pit jiffies is incrementing

    Barrier* b = new Barrier(num_threads + 1);

    for (uint32_t i = 0; i < num_threads; i++) {
        thread([&b] {
            sleep(1);
            Debug::printf("*** Slept 1 seconds\n");
            b->sync();
        });
    }

    b->sync();
    Debug::printf("*** done sleeping\n");

    // Simple bounded buffer test, just create threads that push into the buffer and threads that get from it
    // in an alternating fashion
    BB<uint32_t>* bounded = new BB<uint32_t>(10);
    uint32_t one = 1;
    Barrier* b2 = new Barrier(num_threads + 1);
    for (uint32_t i = 0; i < num_threads; i++) {
        if (i % 2 == 0) {
            thread([&bounded, &one, &b2] {
                bounded->put(one);
                b2->sync();
            });
        }
        else {
            thread([&bounded, &b2] {
                Debug::printf("*** Getting value %d\n", bounded->get());
                b2->sync();
            });
        }
    }
    b2->sync();
    Debug::printf("*** done\n");
}

