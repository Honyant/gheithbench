#include "debug.h"
#include "threads.h"

#define THREADZ 100

void kernelMain(void) {

    //Sleep Implementation test
    //and very slight stress test

    for(int i = 0; i < THREADZ; i++){
        thread([i] {
            sleep(1);
            Debug::printf("*** Threads slept for 1 second!!!!!\n");
            
        });
    }

    for(int i = 0; i < THREADZ; i++){
        thread([i] {
            sleep(2);
            Debug::printf("*** Threads slept for 2 second!!!!!\n");
            
        });
    }

    for(int i = 0; i < THREADZ; i++){
        thread([i] {
            sleep(3);
            Debug::printf("*** Threads slept for 3 second!!!!!\n");
            
        });
    }

    for(int i = 0; i < THREADZ; i++){
        thread([i] {
            sleep(4);
            Debug::printf("*** Threads slept for 4 second!!!!!\n");
            
        });
    }

    for(int i = 0; i < THREADZ; i++){
        thread([i] {
            sleep(5);
            Debug::printf("*** Threads slept for 5 second!!!!!\n");
            
        });
    }

    for(int i = 0; i < THREADZ; i++){
        thread([i] {
            sleep(6);
            Debug::printf("*** Threads slept for 6 second!!!!!\n");
            
        });
    }


     sleep(7);
     Debug::printf("*** KERMAIN MAIN RAWRRRR\n");

}

