#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "promise.h"

/* Called by one core */
void kernelMain(void) {

    auto answer = new Promise<int>();
    auto main_answer = new Promise<int>();

    // thread that sets the value after some work
    thread([answer] {
        Debug::printf("thread 1 is doing some work...\n");
        for (int i = 0; i < 1000000; ++i) {
            if (i % 100000 == 0) {
                yield(); 
                Debug::printf("thread 1 yielding...\n");
            }
        }
        Debug::printf("thread 1 setting answer to 42\n");
        answer->set(42);
    });

    // Thread will set value for main_answer after receiving the promise
    thread([answer, main_answer] {
        Debug::printf("thread 2 is waiting for the answer...\n");
        yield();
        int result = answer->get();
        Debug::printf("*** thread 2 received answer = %d\n", result);
        main_answer->set(1000);
    });

    // main thread also waits for the promise
    Debug::printf("main thread yielding before waiting for answer...\n");
    yield(); 
    Debug::printf("main thread waiting for answer...\n");
    int mainResult = main_answer->get();
    Debug::printf("*** main thread received main_answer = %d\n", mainResult);
}