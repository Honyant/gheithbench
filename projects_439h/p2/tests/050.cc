#include "debug.h"
#include "threads.h"
#include "barrier.h"
#include "atomic.h"
#include "promise.h"

Atomic<int> barrier_counter{0};

void kernelMain() {
    Debug::printf("*** Starting test\n");

    // promises and barriers
    auto addition_done = new Atomic<int>{0};
    auto addition_done_promise = new Promise<int>;
    auto b = new Barrier(10);

    for(int i = 0; i < 10; i++){
        thread([b, addition_done, addition_done_promise] {
            barrier_counter.fetch_add(1); 
            b->sync();
            if(addition_done->exchange(1) == 0){
                addition_done_promise->set(1);
            }
        });
    }
    
    if(addition_done_promise->get() == 1) {
        Debug::printf("*** counter: %d\n", barrier_counter.get()); // 40
    }

    // additional promise test
    int local_int = 0;
    auto promises = new Promise<uint32_t>[10];
    for(int i = 0; i < 10; i++) {
        thread([i, promises] {
            promises[i].set(i);
        });
        
        local_int += promises[i].get();
    }
    Debug::printf("*** %d\n", local_int);

    // dot product
    auto array1 = (int *)malloc(10000 * sizeof(int)); 
    auto array2 = (int *)malloc(10000 * sizeof(int)); 
    for(int i = 0; i < 10000; i++){
        array1[i] = i % 10;
        array2[i] = (i + 5) % 10;
    }

    Atomic<int>* final_dot_product = new Atomic<int>{0};
    auto promise = new Promise<uint32_t>; 

    thread([array1, array2, final_dot_product, promise] {
        for(int i = 0; i < 10000; i++){
            int product = array1[i] * array2[i];
            final_dot_product->fetch_add(product);
        }
        promise->set(final_dot_product->get());
    });
    
    int ans = promise->get(); 

    free(array1);
    free(array2);
    Debug::printf("*** dot product result: %d\n", ans); // # # # #
}