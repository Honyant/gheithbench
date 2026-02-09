#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"
#include "bb.h"

const int NUM_SLEEP_THREADS = 200;

void sleep_test() {
    Barrier* sleep_barrier = new Barrier(NUM_SLEEP_THREADS + 1);
    for (int i = 0; i < NUM_SLEEP_THREADS; i++) {
        thread([sleep_barrier] {
            sleep(1);
            Debug::printf("*** Slept 1 second\n");
            sleep_barrier->sync();
        });
    }
    sleep_barrier->sync();
    Debug::printf("*** done\n");
}

Barrier bounded_barrier(3);
void bounded_buffer_test() {
    BB<int>* bounded_buffer = new BB<int>(2);

    thread([&] {
        for (int i = 0; i < 100; i++) {
            bounded_buffer->put(i);
        }
        bounded_barrier.sync();
    });

    thread([&] {
        for (int i = 0; i < 100; i++) {
            int x = bounded_buffer->get();
            Debug::printf("*** Got %d\n", x);
        }
        bounded_barrier.sync();
    });

    bounded_barrier.sync();
}

void kernelMain(void)
{
    sleep_test(); // simple sleep, threading test
    bounded_buffer_test(); // bounded buffer test
}
