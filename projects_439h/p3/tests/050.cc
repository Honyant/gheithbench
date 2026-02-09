#include <stdint.h>
#include "smp.h"
#include "threads.h"
#include "bb.h"
#include "atomic.h"
#include "random.h"

uint32_t BOUNDED_BUFFER_SIZE = 10;
constexpr int N_PRODUCERS = 2;
constexpr int N_CONSUMERS = 10;
constexpr int N_WORK = 100000;
constexpr int N_WORK_PER_PRODUCER = N_WORK / N_PRODUCERS;
constexpr int N_WORK_PER_CONSUMER = N_WORK / N_CONSUMERS;


Atomic<int> work_done{0};

// This test case tests your bounded buffer in the case of heavy heap access
void kernelMain(void) {

    // This thread repeatedly allocates and deallocates from the heap.
    // Increases the chance of contention for heap operations.
    // Be careful that you don't perform any heap operations inside SpinLocks and regions with disabled interrupts.
    // Reason through every place where you interact with the heap, because this test will stress those regions out
    thread([] {
        Interrupts::disable();
        Random rand{0};
        while (true) {
            auto p1 = malloc(rand.next() * sizeof(double));
            auto p2 = malloc(rand.next() * sizeof(double));
            free(p1);
            free(p2);
        }
        stop();
    });

    BB<int>* bounded_buffer = new BB<int>(BOUNDED_BUFFER_SIZE);

    // Producers
    for (int i = 0; i < N_PRODUCERS; i++) {
        thread([bounded_buffer] {
            for (int i = 0; i < N_WORK_PER_PRODUCER; i++){
                int val = 1;
                bounded_buffer->put(val);
            }
            stop();
        });
    }

    // Consumers
    for (int i = 0; i < N_CONSUMERS; i++) {
        thread([bounded_buffer] {
            for (int i = 0; i < N_WORK_PER_CONSUMER; i++){
                int val = bounded_buffer->get();
                work_done.add_fetch(val);
            }
            stop();
        });
    }

    while (work_done.get() < N_PRODUCERS * N_WORK_PER_PRODUCER) {
        yield();
    }

    Debug::printf("*** You passed this test!\n");
}

