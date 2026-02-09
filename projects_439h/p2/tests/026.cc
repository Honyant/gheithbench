#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "promise.h"
#include "barrier.h"

// Simple stress test to ensure creating threads does not incur extreme overhead

int NUM_THREADS_CREATED = 300;
Barrier *b = new Barrier(NUM_THREADS_CREATED + 1);

void kernelMain(void) {
    Debug::printf("*** start\n");
    for (int i = 0; i < NUM_THREADS_CREATED; i++) {
        thread([]{
            b->sync();
        });
    }
    
    b->sync();
    Debug::printf("*** finish\n");
}