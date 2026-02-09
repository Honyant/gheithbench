#include "debug.h"
#include "shared.h"
#include "threads.h"
#include "promise.h"

void kernelMain(void) {
    Promise<bool>* promise = new Promise<bool>;

    // this code block ensures the strong pointer goes out of scope
    {
        StrongPtr<int> strong = StrongPtr<int>::make(42);
        for (uint32_t i = 0; i < MAX_PROCS; i++) {
            WeakPtr<int> weakPtr = WeakPtr<int>(strong); // one per thread

            // many threads maximize the possibility that two try to promote at once
            thread([weakPtr, promise] () mutable {
                promise->get(); // wait for strong pointer to go out of scope
                while (true) {
                    ASSERT(weakPtr.promote() == nullptr); // no surviving strong pointer: promotion should always fail
                }
            });
        }
    }

    promise->set(true); // strong pointer is now out of scope
    sleep(1); // allow threads to loop for a while
    Debug::printf("*** Test passed! You avoid race conditions when trying to promote two weak pointers at once.\n");
}

