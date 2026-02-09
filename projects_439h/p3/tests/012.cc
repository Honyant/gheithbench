#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "bb.h"
#include "random.h"

/*
 * My test case tests that you implement the bounded buffer destructor to free all associated data.
 * Many implementations of a bounded buffer store bufferd values in a heap allocation proportional to the buffer capacity, which must be freed along with the buffer.
 * In my test case, I repeatedly create and delete large buffers; no heap allocations should persist from iteration to iteration.
 * Ideally, I would just construct and delete the buffers, but the compiler tends to optimize that out, so I have to put things in the buffers.
 * If I just made higher-capacity buffers of integer types (instead of large arrays), iterating through the buffer capacity would take too long.
 * Unfortunately, this test seems not to trigger the usual graceful failure ("out of memory" in the .raw file) and just halts without any warning.
 */

/* Output Key:
 * "": You do not enter kernelMain().
 * "hello": You do not free all of your memory (or you have a bug in the basic case of put()).
 * "hello" and "goodbye": You free your associated data correctly.
 * Anything else: You probably fail some assertion or panic condition in your code.
 */

#define DATA_SIZE 0x100 // much larger would overflow the thread stack :(
#define BUFFER_SIZE 0x100
#define ITERATIONS 0x100

typedef struct Data {
    uint32_t data[DATA_SIZE]; // I have to wrap this in a struct or the compiler complains that get() returns an array :(
} Data;

void kernelMain(void) {
    Debug::printf("*** hello\n");
    for (uint32_t i = 0; i < ITERATIONS; i++) {
        BB<Data>* bb = new BB<Data>{BUFFER_SIZE};
        Data d = {};
        for (int j = 0; j < BUFFER_SIZE; j++) {
            bb->put(d); // if I don't do this the compiler optimizes out the buffer :(
        }
        delete bb;
    }
    Debug::printf("*** goodbye\n");
}

