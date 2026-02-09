#include "debug.h"
#include "threads.h"
#include "bb.h"
#include "barrier.h"
#include "atomic.h"

constexpr int BUFFER_SIZE = 6;
constexpr int PRODUCERS = 2;
constexpr int CONSUMERS = 2;

/*
* This test tests reusing of a boundedbuffer. Once it hits it max capacity, it is emptied out.
* it should be able to be reused. This test essentially insures that you index your 
* circular buffer correctly
*/
/* Called by one core */
void kernelMain(void) {

    Debug::printf("*** Bounded Buffer Test Start ***\n");

    auto bb = new BB<int>(BUFFER_SIZE);
    auto pbarrier = new Barrier(PRODUCERS + 1);
    auto cbarrier = new Barrier(CONSUMERS + 1);
    Atomic<int> total_sum_produced{0};
    Atomic<int> total_sum_consumed{0};

    // Producer threads
    for (int i = 0; i < PRODUCERS; i++) {
        thread([bb, pbarrier, &total_sum_produced, i] {
            for (int j = 0; j < BUFFER_SIZE/PRODUCERS; j++) {
                int value = j;
                bb->put(value);
                total_sum_produced.fetch_add(value);
            }
            pbarrier->sync();
        });
    }
    pbarrier->sync();

    Debug::printf("*** Finished Producers (1)\n");
    // Consumer threads
    for (int i = 0; i < CONSUMERS; i++) {
        thread([bb, cbarrier, &total_sum_consumed] {
            for (int j = 0; j < BUFFER_SIZE / CONSUMERS; j++) {
                int value = bb->get();
                total_sum_consumed.fetch_add(value);
            }
            cbarrier->sync();
        });
    }

    cbarrier->sync();

    Debug::printf("*** First Round of Put/Get Completed ***\n");

    int expected_sum = PRODUCERS * (BUFFER_SIZE/PRODUCERS) * (BUFFER_SIZE/PRODUCERS - 1) / 2;

    Debug::printf("*** Total sum produced: %d (expected %d)\n", total_sum_produced.get(), expected_sum);
    Debug::printf("*** Total sum consumed: %d (expected %d)\n", total_sum_consumed.get(), expected_sum);

    // Test reusing the buffer
    Atomic<int> reuse_sum_produced{0};
    Atomic<int> reuse_sum_consumed{0};

    auto barrier1 = new Barrier(2);
    thread([bb, barrier1, &reuse_sum_produced] {
        for (int i = 0; i < BUFFER_SIZE; i++) {
            int value = i;  // New set of values for reuse test
            bb->put(value);
            reuse_sum_produced.fetch_add(value);
        }
        barrier1->sync();
    });

    barrier1->sync();

    auto barrier2 = new Barrier(2);
    thread([bb, barrier2, &reuse_sum_consumed] {
        for (int i = 0; i < BUFFER_SIZE; i++) {
            int value = bb->get();
            reuse_sum_consumed.fetch_add(value);
        }
        barrier2->sync();
    });

    barrier2->sync();

    Debug::printf("*** Reuse Round Completed ***\n");

    // Expected sum for reuse test
    int expected_reuse_sum = (BUFFER_SIZE) * (BUFFER_SIZE - 1)/2;

    Debug::printf("*** Reused sum produced: %d (expected %d)\n", reuse_sum_produced.get(), expected_reuse_sum);
    Debug::printf("*** Reused sum consumed: %d (expected %d)\n", reuse_sum_consumed.get(), expected_reuse_sum);

    Debug::printf("*** Bounded Buffer Test Completed Successfully ***\n");
}