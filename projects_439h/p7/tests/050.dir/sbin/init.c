#include "libc.h"
#include <stdarg.h>
#include "stdbool.h"

/*
This test stresses exists and verifies that exit successfully closes the process.
This includes:
- Releasing all private virtual memory mappings
- Releasing all semaphores
- Releasing all child processes
*/

int main(int argc, char** argv) {
    printf("*** %d\n",argc);
    for (int i=0; i<argc; i++) {
        printf("*** %s\n",argv[i]);
    }

    printf("*** Attempting to fork 100 processes\n");
    // Fork
    for (int i = 0; i < 100; i++) {
        int child_id = fork();
        if (child_id < 0) {
            printf("*** Fork failed at attempt %d\n", i);
            shutdown();
            return 0;
        } else if (child_id == 0) {
            // Child
            exit(0);
        } else {
            // Parent
            uint32_t c = close(child_id);
            if (c != 0) {
                printf("*** Close failed for child %d\n", child_id);
                shutdown();
                return 0;
            }
        }
    }
    printf("*** Forked and closed 100 child\n");

    for (int i = 0; i < 100; i++) {
        int child_id = fork();
        if (child_id < 0) {
            printf("*** Fork failed at attempt %d\n", i);
            shutdown();
            return 0;
        } else if (child_id == 0) {
            // Child
            exit(0);
        }
    }
    printf("*** Forked 100 more children: YAY!\n");

    shutdown();
    return 0;
}