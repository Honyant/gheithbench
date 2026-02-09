#include "libc.h"

void one(int fd) {
    printf("*** fd = %d\n",fd);
    cp(fd,2);
}

int main(int argc, char** argv) {
    printf("*** %d\n",argc);

    // test single file descriptor - open, close, copy

    int fd_test = open("/etc/oner.txt",0);
    one(fd_test);
    printf("*** close = %d\n",close(fd_test));

    // test multiple file descriptors - open, close, copy
    // test that the lowest file descriptor is allotted each time

    int fd_3 = open("/etc/oner.txt",0);
    int fd_4 = open("/etc/twoer.txt",0);
    int fd_5 = open("/etc/threer.txt",0);

    one(fd_3);
    one(fd_4);
    one(fd_5);

    printf("*** close = %d\n",close(fd_4));
    printf("*** close = %d\n",close(fd_5));

    int fd_4b = open("/etc/oner.txt",0);
    one(fd_4b);

    printf("*** close = %d\n",close(fd_4b));
    printf("*** close = %d\n",close(fd_3));

    // testing limit for file descriptor and semaphore creation

    for (int i = 0; i < 97; i++) {
        int fd_limit = open("/etc/oner.txt",0);
        if (fd_limit < 0) {
            printf("*** ooops\n");
        }
    }

    for (int i = 0; i < 100; i++) {
        int sem_limit = sem(1);
        // also tests semaphore id >= 100
        if (sem_limit < 100) {
            printf("*** ooops\n");
        }
    }

    printf("*** limit test done\n");

    // test basic fork

    int id = fork();
    if (id < 0) {
        printf("fork failed");
    } else if (id == 0) {
        /* child */
        printf("*** child\n");
        exit(0);
    } else {
        /* parent */
        uint32_t status = 42;
        wait(id,&status);
        printf("*** parent\n");
    }

    printf("*** basic forker done\n");
    // test complex fork

    int ida = fork();
    if (ida < 0) {
        printf("fork failed");
    } else if (ida == 0) {
        /* parent */
        printf("*** parent\n");

        int idb = fork();
        if (idb < 0) {
            printf("fork failed");
        } else if (idb == 0) {
            /* child */
            printf("*** child\n");
            exit(0);
        } else {
            /* parent */
            uint32_t status = 42;
            wait(idb,&status);
            printf("*** parent\n");
        }
        exit(0);
    } else {
        /* grandparent */
        uint32_t status = 42;
        wait(ida,&status);
        printf("*** grandparent\n");
    }

    

    shutdown();
    return 0;
}
