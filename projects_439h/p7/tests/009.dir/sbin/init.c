#include "libc.h"

#define FORK_COUNT 6
int main(int argc, char** argv) {
    // Welcome! This is a simple test for the fork() and semaphore system calls, as well as exploration of true nature of the self!
    printf("*** made it to main!\n");
    int identity = 0;

    int semaphores[1 << FORK_COUNT] = {0};
    for (int i=0; i<(1 << FORK_COUNT); i++) {
        semaphores[i] = sem(0);
    }

    for (int i=0; i<FORK_COUNT; i++) {
        identity |= ((int)(fork() == 0)) << i;
    }

    if (identity == 0) {
        printf("*** I am the original article!\n");
        printf("*** Only one can remain! >:)\n");
        up(semaphores[1]);
        down(semaphores[0]);
        // Killed by my own clone, how ironic
        exit(0);
    } else {
        down(semaphores[identity]);
        printf("*** I am clone #%d, and I have been ordered to die :'(\n",identity);
        if (identity < (1 << FORK_COUNT) - 1) {
            up(semaphores[identity+1]);
            exit(0);
        } else {
            printf("*** I am the last clone standing! >:( Now, I must avenge my fallen brethren, and destroy the original!\n");
            up(semaphores[0]);
            shutdown();
        }
    }   
}
