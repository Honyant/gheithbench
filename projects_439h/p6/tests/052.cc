#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"
#include "heap.h"
#include "barrier.h"

//Tests small optimization -- makes sure you don't allocate too much memory to data structure upon a single mmap
//If you do, you will run out of memory

int NUM_THREADS = 631;
int BREATHING_ROOM = 6;
Barrier b{NUM_THREADS - BREATHING_ROOM + 1};

/* Called by one CPU */
void kernelMain(void) {

    
    using namespace VMM;
    for(int i = 0; i < NUM_THREADS - BREATHING_ROOM; i++){
        thread([]{
            naive_mmap(1, false, StrongPtr<Node>{}, 0);
            b.sync();
        });
    }
    b.sync();
    Debug::printf("*** yay you have really good code\n");


}
