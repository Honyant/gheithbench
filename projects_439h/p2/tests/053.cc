#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "promise.h"
#include "barrier.h"

constexpr uint32_t N = 10;
Atomic<uint32_t> counter{0};

void kernelMain(void) {

    auto b1 = new Barrier(N);
    auto b2 = new Barrier(N);
    auto between3 = new Barrier(N);
    auto between1 = new Barrier(N);
    auto between2 = new Barrier(N);
    auto b_done = new Barrier(3 * N + 1);

    for (uint32_t i = 0; i < N; i++) {
        thread([b1, b2, between1, between2, between3, b_done]{

            counter.add_fetch(1);
            b1->sync();
            Debug::printf("*** %d\n", counter.get());

            between1->sync();
            thread([b2, between2, between3, b_done]() {
                counter.add_fetch(1);
                b2->sync();
                Debug::printf("*** %d\n", counter.get());\

                between2->sync();
                thread([between2, between3, b_done] {
                    counter.add_fetch(1);
                    between3->sync();
                    Debug::printf("*** %d\n", counter.get());\
                    
                    b_done->sync();
                });
                b_done->sync();
            });
            b_done->sync();
        });
    }

    b_done->sync();
}

