#include "libc.h"
#include "promise.h"

int main(int argc, char** argv) {
    // spawn children in a loop and if they are something then something
    int pid = 1;
    Promise *promise = (Promise *) malloc(sizeof(Promise));
    Promise_init(promise);
    pid = fork();

    if (pid == 0) { // I am a child, wait on the promise
        Promise_get(promise);
        return 99;
    }
    else {
        printf("*** about to set promise\n");
        uint32_t num = 0;
        Promise_set(promise, num);
        uint32_t status;
        int wait_status = wait(pid, &status);
        printf("*** parent wait status: %d, child exit status: %ld\n", wait_status, status);
    }

    free((void *) promise);
    return 100;
}
