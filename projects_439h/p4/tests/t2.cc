#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "promise.h"

/* Called by one core */
void kernelMain(void) {

    auto answer = new Promise<int>();

    thread([answer] {
        answer->set(42);
    });

    

    Debug::printf("*** answer = %d\n",answer->get());

}

