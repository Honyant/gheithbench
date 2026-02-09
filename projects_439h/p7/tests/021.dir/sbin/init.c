#include "libc.h"

void one(int fd) {
    printf("*** fd = %d\n",fd);
    printf("*** len = %d\n",len(fd));

    cp(fd,2);
}

int main(int argc, char** argv) {
    printf("*** %d\n",argc);
    for (int i=0; i<argc; i++) {
        printf("*** %s\n",argv[i]);
    }

    // TEST 1: Basic Semaphore Test
    uint32_t num1 = 0;
    int id = fork();

    if (id == 0) {
        execl("/sbin/t1", "t1", "This", "is", "a", "basic", "semaphore", "test", 0);
    }
    else {
        wait(id, &num1);
        printf("*** Test %ld completed\n", num1);
    }


    // TEST 2: Child disowning
    uint32_t num2 = 2;
    int id2 = fork();

    if (id2 == 0) {
        return 100;
    }  
    else {
        close(id2);
        wait(id2, &num2);
        // if you didn't disown properly, this will print 100
        printf("*** Test %ld completed\n", num2);
    }

    shutdown();
    return 0;
}
