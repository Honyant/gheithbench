#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "bb.h"

// This test case is a stress test for bounded buffers.

constexpr uint32_t PRODUCER_COUNT = 100;
constexpr uint32_t CONSUMER_COUNT = 100;
constexpr uint32_t ITEMS_PER_PRODUCER = 1000;
constexpr uint32_t TOTAL_ITEMS = PRODUCER_COUNT * ITEMS_PER_PRODUCER;
constexpr uint32_t BUFFER_SIZE = 50;
Atomic<uint32_t> produced_count{0};
Atomic<uint32_t> consumed_count{0};

void kernelMain(void)
{
    Debug::printf("*** Starting Bounded Buffer Stress Test\n");

    BB<int> *bb = new BB<int>(BUFFER_SIZE);

    // Start producer threads
    for (uint32_t i = 0; i < PRODUCER_COUNT; i++)
    {
        thread([bb, i]
               {
            for (uint32_t j = 0; j < ITEMS_PER_PRODUCER; j++) {  // Changed to uint32_t
                int item = i * ITEMS_PER_PRODUCER + j;
                bb->put(item);
                produced_count.fetch_add(1);
            } });
    }
    Debug::printf("*** Finished the producers\n");

    // Start consumer threads
    for (uint32_t i = 0; i < CONSUMER_COUNT; i++)
    {
        thread([bb, i]
               {
            for (uint32_t j = 0; j < ITEMS_PER_PRODUCER; j++) {  // Changed to uint32_t
                bb->get();
                consumed_count.fetch_add(1);
            } });
    }
    Debug::printf("*** Finished the consumers\n");

    // Wait until all items have been produced and consumed
    while (produced_count.get() < TOTAL_ITEMS || consumed_count.get() < TOTAL_ITEMS)
    {
        yield();
    }

    Debug::printf("*** Stress Test Complete: %d items produced, %d items consumed\n",
                  produced_count.get(), consumed_count.get());

    ASSERT(produced_count.get() == TOTAL_ITEMS);
    ASSERT(consumed_count.get() == TOTAL_ITEMS);

    Debug::printf("*** Bounded Buffer Stress Test Passed\n");
}
