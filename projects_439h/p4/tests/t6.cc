#include "debug.h"
#include "threads.h"


/* Called by one core */
void kernelMain(void) {

    thread([] {
        sleep(2);
        Debug::printf("*** 2\n");
    });
    
    thread([] {
        sleep(1);
        Debug::printf("*** 1\n");
    });

    sleep(3);
    Debug::printf("*** 3\n");

}

