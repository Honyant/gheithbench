#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"
#include "promise.h"
#include "bb.h"

//Stress test on bounded buffer
//Short and easy to debug


Atomic<uint32_t> count = 0;
const uint32_t N = 499;
//Barrier* b = new Barrier(N + 1);
Barrier* b2 = new Barrier(N + 1);
Atomic<uint32_t> result = 0;
Atomic<uint32_t> onlyoneprint = 0;
BB<int>* p = new BB<int>(1);

/* Called by one core */
void kernelMain(void) {
    if(count.fetch_add(1) <= N){
        int i = count.get();
        thread(&kernelMain);
        p->put(i);
        //b->sync();
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
    
    

