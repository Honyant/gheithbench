#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"
#include "promise.h"

//Stress test + makes sure you are storing thread data like promises on the stack
//Short and easy to debug


Atomic<uint32_t> count = 0;
const uint32_t N = 499;
Barrier* b = new Barrier(N + 1);
Barrier* b2 = new Barrier(N + 1);
Atomic<uint32_t> result = 0;
Atomic<uint32_t> onlyoneprint = 0;

/* Called by one core */
void kernelMain(void) {
    if(count.fetch_add(1) <= N){
        auto p = new Promise<int>();
        p->set(count);
        thread(&kernelMain);
        b->sync();
        result.add_fetch(p->get()); //Supposed to add all numbers from 1 to N + 1
        b2->sync();
        if(!onlyoneprint.exchange(1)){
            Debug::printf("*** %d\n", result.get());
            Debug::shutdown();
        }
    }
    if(count.get() >= N + 1)
        stop();
}
    
    

