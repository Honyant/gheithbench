#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "promise.h"
#include "barrier.h"
#include "bb.h"

#define THREADS 100
/* Called by one core */
void kernelMain(void) {

    // Let's start simple. Testing your promise and barriers.
    auto prom = new Promise<int>();
    auto b = new Barrier(THREADS + 1);
    prom->set(0);

    for (int i = 0; i < THREADS; i++) {
        thread([prom, b]{
            prom->get();
            b->sync();
        });
    }

    b->sync();

    Debug::printf("*** I promise your barrier and promises are (probably) correct.\n");

    // Testing your bounded buffers and stess testing sleep.
    auto buffer = new BB<int>(THREADS);
    auto b2 = new Barrier(2 * THREADS + 1);
    for (int i = 0; i < THREADS; i++) {
        thread([b2, buffer]{
            int a = 1;
            sleep(1);
            buffer->put(a); // sleep before put
            b2->sync(); 
        });
    }

    for (int i = 0; i < THREADS; i++) {
        thread([b2, buffer]{
            sleep(1);
            buffer->get(); // sleep before and after get
            sleep(1);
            b2->sync();
        });
    }

    b2->sync();
    Debug::printf("*** All your threads moved on! Your bounded buffer and sleep are probably correct and (decently) fast.\n");

}

