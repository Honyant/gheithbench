#include "libc.h"
#include "debug.h"

void close_with_check(int fd) {
    int status = close(fd);
    ASSERT(status == 0);
}

/*
This test case tests your chdir implementation, and ensure you handle all edge cases of switching directories.
*/
int main(int argc, char** argv) {
    printf("*** Beginning chdir test\n");

    printf("*** First Test, Basic Open and Read\n");
    int fd1 = open("/homophone.txt",0);
    ASSERT(fd1 == 3);
    cp(fd1, 1);

    printf("*** Second Test, chdir to /dir, absolute path\n");
    chdir("/dir1");
    int fd2 = open("homophone.txt",0);
    ASSERT(fd2 == 4);
    cp(fd2, 1);

    printf("*** Third Test, chdir to /dir1/dir2, absolute path\n");
    chdir("/dir1/dir2");
    int fd3 = open("homophone.txt",0);
    ASSERT(fd3 == 5);
    cp(fd3, 1);

    printf("*** Fourth Test, chdir back to root (/), absolute path\n");
    chdir("/");
    int fd4 = open("homophone.txt",0);
    ASSERT(fd4 == 6);
    cp(fd4, 1);

    printf("*** Fifth Test, chdir to /dir1, relative path from root\n");
    chdir("dir1");
    int fd5 = open("homophone.txt",0);
    ASSERT(fd5 == 7);
    cp(fd5, 1);

    printf("*** Sixth Test, chdir to (dir1/)/dir2, relative path from dir1\n");
    chdir("dir2");
    int fd6 = open("homophone.txt",0);
    ASSERT(fd6 == 8);
    cp(fd6, 1);

    printf("*** Seventh Test, chdir back to root (/) (../../), relative path from dir2\n");
    chdir("../../");
    int fd7 = open("homophone.txt",0);
    ASSERT(fd7 == 9);
    cp(fd7, 1);

    printf("*** Eighth Test, chdir to /dir1, complex absolute path of and ., ..'s\n");
    chdir("/dir1/.././dir1/dir2/../../dir1/.././dir1/dir2/../../dir1/.././dir1/dir2/../../dir1/.././dir1/dir2/../../dir1/.././dir1/dir2/../../dir1");
    int fd8 = open("homophone.txt",0);
    ASSERT(fd8 == 10);
    cp(fd8, 1);

    printf("*** Ninth Test, chidir to symDir2 (points to dir2)\n");
    chdir("/symDir2/");
    int fd9 = open("homophone.txt",0);
    ASSERT(fd9 == 11);
    cp(fd9, 1);

    printf("*** Tenth Test, chdir to invalidDir\n");

    int status = chdir("/invalidDir");
    ASSERT(status < 0);
    printf("*** PASS!\n");

    printf("*** Last Test, closing all fds\n");
    
    close_with_check(fd1);
    close_with_check(fd2);
    close_with_check(fd3);
    close_with_check(fd4);
    close_with_check(fd5);
    close_with_check(fd6);
    close_with_check(fd7);
    close_with_check(fd8);

    printf("*** CHDIR TESTS PASSED\n");

    shutdown();
    return 0;
}
