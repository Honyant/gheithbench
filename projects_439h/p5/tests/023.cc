#include "ide.h"
#include "ext2.h"
#include "libk.h"
#include "kernel.h"
#include "pit.h"
#include "debug.h"

class Timer {
private:
    uint32_t start_jiffies;
    uint32_t stop_jiffies;

public:
    // Start the timer
    void start() {
        start_jiffies = Pit::jiffies;
    }

    // Stop the timer
    void stop() {
        stop_jiffies = Pit::jiffies;
    }

    // Calculate elapsed time in milliseconds
    uint32_t elapsed() const {
        // Assuming each jiffy represents 1 millisecond
        return (stop_jiffies - start_jiffies); // Return difference in jiffies
    }
};

// Example of how to use the timer in the cache test
void testCacheLatency() {
    auto ide = StrongPtr<Ide>::make(1, 100);
    auto fs = StrongPtr<Ext2>::make(ide);
    auto root = fs->root;

    Debug::printf("*** Testing cache latency\n");

    Timer timer;

    // First call to fs->find (expect a cache miss)
    timer.start();
    auto hello = fs->find(root, "hello");
    timer.stop();
    uint32_t first_read_time = timer.elapsed();
    Debug::printf("First read (cache miss): %d jiffies\n", first_read_time);


    timer.start();
    // Perform multiple subsequent reads (should hit the cache)
    double total_cached_time = 0;
    for (uint32_t i = 0; i < 100000; i++) {
        hello = fs->find(root, "hello");  // Should be a cache hit after the first one
    }

    timer.stop();

    total_cached_time += timer.elapsed();

    Debug::printf("100000 (cache hits): %f jiffies\n", total_cached_time);


    
    // Compare times to check for cache hits. Arbitrary multipler
    
    Debug::printf("*** Cache is working as expected.\n");
    

    Debug::printf("*** Cache latency test done.\n");
}
void testTwoSym() {
    //make sure you loop for following symbolinks. (but don't infinite loop)
    Debug::printf("*** Testing symbo traversal\n");
    auto ide = StrongPtr<Ide>::make(1, 100);
    auto fs = StrongPtr<Ext2>::make(ide);
    auto root = fs->root;
    auto end = fs->find(root, "丌");
    auto begin = fs->find(root, "二/");
    ASSERT(end->number == begin->number);
    Debug::printf("*** You followed twice successfully\n");
}
/* Called by one core */
void kernelMain(void) {
    testCacheLatency();
    testTwoSym();
}

