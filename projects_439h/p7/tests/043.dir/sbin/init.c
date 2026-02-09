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

    printf("*** Test 1: Basic file open, read, close\n");
    int fd = open("/harrypotter/harrypotter1.txt", 0);
    if (fd == 3) printf("*** assigned fd to %d\n", fd);
    one(fd);
    printf("*** close = %d\n",close(fd));

    printf("*** Test 2: Closing std::in, putting a file, reading from that file\n");
    close(0);
    fd = open("/harrypotter/harrypotter1.txt", 0);
    if (fd == 0) printf("*** assigned fd to %d\n", fd);
    one(0);
    printf("*** close = %d\n",close(fd));

    printf("*** Test 3: When fork child shouldn't have std::in\n");
    int id = fork();

    if (id == 0) {
        // Child should be able to put into 0
        printf("*** Child\n");
        fd = open("/harrypotter/harrypotter1.txt", 0);
        if (fd == 0) printf("*** assigned fd to %d\n", fd);
        one(0);
        exit(0);
    }
    else if (id > 0) {
        // Parent should be able to put into 0
        uint32_t status = 42;
        wait(id, &status);
        printf("*** Parent\n");
        fd = open("/harrypotter/harrypotter1.txt", 0);
        if (fd == 0) printf("*** assigned fd to %d\n", fd);
        printf("*** close = %d\n",close(fd));
    }

    shutdown();
    return 0;
}
