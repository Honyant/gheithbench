#include "libc.h"

int main(int argc, char** argv) {
    printf("*** %x\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("*** %s\n", argv[i]);
    }

    int sem_id = sem(0);
    int child_id = fork();

    if (child_id == 0) {
        printf("*** first\n");
        up(sem_id);
        return 0;
    }
    else if (child_id > 0) {
        down(sem_id);
        printf("*** second\n");
    }
    else {
        printf("error\n");
        shutdown();
    }

    // shouldn't work
    close(sem_id);

    return 1;
}