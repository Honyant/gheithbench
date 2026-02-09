#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "bb.h"
#include "barrier.h"

Atomic<uint32_t> thread_step{0};
Atomic<uint32_t> main_step{0};

/* Called by one core */
void kernelMain(void) {
    Debug::printf("*** hello\n");

    // sleep test - can kernelMain wake up if its the only thread alive?
    sleep(1);

    // Bounded Buffer stress test
    BB<uint32_t> *my_bb = new BB<uint32_t>(100);
    Barrier *barrier1 = new Barrier(102);
    Barrier *barrier2 = new Barrier(102);
    
    for (uint32_t i = 0; i < 101; i++) // one thread may get blocked
    {
        thread([my_bb, barrier1, barrier2] {
            uint32_t num = 10;
            my_bb->put(num);
            barrier1->sync();
            my_bb->get(); // this thread unblocks the one blocked one
            barrier2->sync();
        });
    }

    my_bb->get(); // this thread unblocks the one blocked one

    barrier1->sync(); // wait until bounded buffer is full

    uint32_t num2 = 10;
    my_bb->put(num2); // unblocks the extra thread again

    barrier2->sync(); // wait until bounded buffer is empty

    Debug::printf("*** halfway\n");

    // Pre-emption test
    bool was = Interrupts::disable();

    uint32_t id = SMP::me();
    bool can_move_on = false;
    for (uint32_t i = 0; i < 50; i++)
    {
        thread([id, &can_move_on] {
        while(true) {
                bool was2 = Interrupts::disable();
                if (id == SMP::me()) {
                    can_move_on = true;
                }
                Interrupts::restore(was2);
            }
        });
    }

    ASSERT(Interrupts::isDisabled());

    Interrupts::restore(was);

    while (!can_move_on);

    ASSERT(!Interrupts::isDisabled());

    Debug::printf("*** goodbye\n");
    Debug::shutdown();
}