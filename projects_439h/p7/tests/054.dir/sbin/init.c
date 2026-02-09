#include "libc.h"
/*
4. I open something else, and expect the fd to be 1.
5. I close stderr, write to fd 2, and expect -1.
*/

int main(int argc, char** argv) {

    //1. I close stdin, open something, read from it, and expect the fd to be 0.
    printf("*** close = %d\n",close(0));
    int fd = open("/etc/data.txt",0);
    printf("*** File desc 0: %x\n", fd);
    cp(fd,1);
    seek(fd,0);
    cp(fd,2);
    seek(fd,0);
    //2. close stdout and write to fd 1, expecting -.
    //would no longer be able to use printf after this, but I modified putchar to use stderr.

    printf("*** close1 = %d\n",close(1));

    cp(fd,1);
    seek(fd,0);
    cp(fd,2);
    seek(fd,0);
    //3. open something in fd 1.
    int fd2 = open("/etc/panic.txt",0);
    printf("*** File desc 1: %x\n", fd2);
    if (write(1,&fd2,1) >= 0) {
        shutdown();
    }
    cp(fd2,2);
    seek(fd2,0);
    // //4. close stderr and write to fd 2, expecting -.
    close(2); //we literally can't print anything anymore.
    shutdown();
    return 0;
}
