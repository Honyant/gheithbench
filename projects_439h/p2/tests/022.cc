#include <stdint.h>
#include "smp.h"
#include "debug.h"
#include "config.h"
#include "threads.h"
#include "promise.h"
#include "barrier.h"

constexpr int N = 10;

/* promises that rely on each other*/

/* Called by one core */
void kernelMain(void) {
    Promise<int>* p[N];
    for (int i = 0; i < N; ++i) {
        p[i] = new Promise<int>();
    }

    Atomic<uint16_t> counter{(N-1)*N*2};


    for (int i = 0; i < N-1; ++i) {
        for (int j = 0; j < N; ++j) {
            thread([i, j, &p, &counter] {
                // Debug::printf("[%d] %d, **%d** wait for promise\n", SMP::me(), i, j); 
                p[j]->get();
                // Debug::printf("[%d] %d, **%d** promise fulfilled: %d\n", SMP::me(), i, j, v); 
                counter.add_fetch(-1);
                
            });
            thread([i, j, &p, &counter] {
                // Debug::printf("%d seeking to get %d promise\n", i, i+1);
                if (i == j) p[i]->set(p[i+1]->get()+1);
                else {
                    p[i+1]->get();
                }
                counter.add_fetch(-1);
            });
            
        }
    }
    thread([&p] {
        Debug::printf("*** set first promise to 0\n");
        p[N-1]->set(0);
    });

    while(counter.get() > 0) yield(); // all promises must finish

    for (int i = 0; i < N; ++i) {
        Debug::printf("*** %d\n", p[i]->get());
    }


    Debug::printf("*** done !!\n");

}

