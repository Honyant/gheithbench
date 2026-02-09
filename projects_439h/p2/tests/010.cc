#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"
#include "promise.h"

#define total_threads 584
const int CHILD_COUNT = 8; // Leads to 584 total threads plus the main thread. Any more runs out of memory 😔
const int RECURSION_DEPTH = 2; // Actually 3, but the main thread is the first one
const int THREAD_LIFESPAN = 10;
Atomic<uint32_t> thread_count{0};
uint8_t* thread_age;
Promise < int > promise {};

void inherit_thread(int count){
    // Let's make sure you're cleaning up after yourself
    thread([count]{
        // Debug::printf("Inheriting thread %u\n", count);
        while (true){
            thread_age[count] += 1;
            if(thread_age[count] >= THREAD_LIFESPAN)
                inherit_thread(count);
            yield();
        }
    });
    stop();
}

void start_threads(int depth){
    for (int i = 0; i < CHILD_COUNT; i++) {
        // Let's see if you can create threads within threads
        thread([depth] {
            uint32_t count = thread_count.fetch_add(1);
            // Debug::printf("Created threads %u\n", count);
            if(depth > 0)
                start_threads(depth - 1);
            if (count == total_threads - 1) {
                // If I'm the final thread, fulfill that promise
                promise.set(1234);
            }
            while (true){
                // We can't let these threads live forever
                thread_age[count] += 1;
                if(thread_age[count] >= THREAD_LIFESPAN)
                    inherit_thread(count);
                yield();
            }
        });
        yield();
    }
}

void kernelMain(void) {
    // This test case deals with thread starvation, 
    // where threads are created and destroyed in a loop with
    // only a single one capable of letting the program exit.
    // Just for fun, we also have the threads be created recursively just to make sure
    // you can create threads outside of the main thread.
    Debug::printf("*** hello\n");
    thread_age = (uint8_t*) (sizeof(uint8_t) * total_threads);
    start_threads(RECURSION_DEPTH);
    promise.get();
    Debug::printf("*** done :)\n");
}
