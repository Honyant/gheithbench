#include "libc.h"
//Easy to debug semaphore test

int main(int argc, char** argv) {
    uint32_t semaphore = sem(1);
    if(semaphore < 100){
        printf("*** wrong sem1 num\n");
    }
    int IPC = open("/dummyfile.txt", 0);
    if(IPC != 3){
        shutdown();
    }
    uint32_t* sharedmap = (uint32_t*) naive_mmap(8, 1, IPC, 0);
    sharedmap[0] = 8; //Used for a barrier of sorts
    fork();
    fork();
    fork(); //Should be 8 processes at the end of this
    down(semaphore);
    printf("*** no race condition\n");
    printf("*** yay\n");
    sharedmap[0]--;
    up(semaphore);
    while(sharedmap[0] != 0);
    shutdown();
    return 0;
}
