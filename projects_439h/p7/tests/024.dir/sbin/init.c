#include "libc.h"
#include "debug.h"

/*
This is mostly a chdir test which tests whether chdir can handle different
types of paths, such as relative paths and absolute paths. It also tests
seek and read to print from a file at an offset. Finally, it tests some
invalid behaviors which are defined by the spec and how they should be handled.
*/

int main(int argc, char** argv) {
    printf("*** %d\n",argc);
    for (int i=0; i<argc; i++) {
        printf("*** %s\n",argv[i]);
    }

    // relative open to root dir
    int fd1 = open("files/prompt.txt",0);
    cp(fd1, 1);

    // ch dir to new dir (relative)
    int status = chdir("october");
    ASSERT(status == 0);
    // test open from there
    int fd2 = open("snack.txt",0);
    cp(fd2, 1);

    // test open using ..
    int fd3 = open("../october/drink.txt",0);
    cp(fd3, 1);

    // test chdir /november (absolute)
    status = chdir("/november");
    ASSERT(status == 0);
    // test open from there
    int fd4 = open("snack.txt",0);
    cp(fd4, 1);

    int fd5 = open("drink.txt",0);
    // seek and read test
    seek(fd5, 22);
    char buff[24];
    read(fd5, buff, 24);
    printf("%s", buff);

    // test some closes are < 0 
    printf("*** close = %d\n",close(fd1));
    printf("*** close = %d\n",close(fd2));
    printf("*** close = %d\n",close(fd3));
    printf("*** close = %d\n",close(fd4));
    printf("*** close = %d\n",close(fd5));
    ASSERT(close(fd1) < 0);

    // file that doesn't exist would create errors
    int err1 = open("err.txt",0);
    ASSERT(err1 < 0);

    // close stdout and stderr
    close(1);
    close(2);
    // this should NOT print
    printf("*** spooky spooky error\n");

    shutdown();
    return 0;
}
