#include "libc.h"

/*
This test case tests various open/close conditions including closing stderr and opening relative paths (with the use of chdir to modify current working directory). This test also ensures reading a file returns the right amount of bytes read
*/
int main(int argc, char** argv) {
    printf("*** %d\n",argc);
    for (int i=0; i<argc; i++) {
        printf("*** %s\n",argv[i]);
    }
    int fd = open("/file1.txt",0);
    if (fd != 3) {
        printf("*** Incorrect FD returned: %d\n", fd);
    } else {
        printf("*** Correct FD returned: %d\n", fd);
    }

    // testing relative path open() as well using chdir, in addition to absolute path open() testing
    int fd2 = open("/file2.txt",0);
    if (fd2 != 4) {
        printf("*** Incorrect FD returned: %d\n", fd2);
    } else {
        printf("*** Correct FD returned: %d\n", fd2);
    }
    open("/file3.txt",0);
    open("/dir1a/dir1b/file4.txt",0);
    chdir("/dir1a/"); // changing cwd
    int fd5 = open("dir1b/dir1c/file5.txt",0);
    if (fd5 != 7) {
        printf("*** Incorrect FD returned: %d\n", fd5);
    } else {
        printf("*** Correct FD returned: %d\n", fd5);
    }
    open("/dir1a/dir1b/dir1d/file6.txt",0);
    chdir("/dir1a/dir1b/dir1d/"); // changing cwd again
    int fd7 = open("dir1e/dir1f/file7.txt",0);
    if (fd7 != 9) {
        printf("*** Incorrect FD returned: %d\n", fd7);
    } else {
        printf("*** Correct FD returned: %d\n", fd7);
    }

    // reading some files to ensure the right number of bytes are read and returned
    uint32_t lenOfFd5 = len(fd5);
    char* buf = malloc(lenOfFd5 + 1);
    uint32_t bytesRead = read(fd5, buf, 1000); // should return lenOfFd5 instead of 1000
    printf("*** Bytes read in fd5: %ld\n", bytesRead);

    uint32_t lenOfFd7 = len(fd7);
    char* buf2 = malloc(lenOfFd7 + 1);
    bytesRead = read(fd7, buf2, 1000); // should return lenOfFd7 instead of 1000
    printf("*** Bytes read in fd7: %ld\n", bytesRead);

    // testing closing multiple FD's consecutively to test for memory leaks and also including stderr to ensure a new opened FD would claim #2 (stderr)
    close(fd2);
    close(fd);
    int32_t res = close(2);
    if (res != 0) {
        printf("*** Close() did not successfully close stderr!\n");
    }
    fd = open("/file1.txt",0); // must claim #2 (old stderr)
    fd2 = open("/file2.txt",0);
    if (fd == 2) {
        printf("*** You correctly set new opened file's FD to replace stderr: %d\n", fd);
    } else  {
        printf("*** You incorrectly set new opened file's FD to replace stderr: %d\n", fd);
    }
    if (fd2 == 3) {
        printf("*** You correctly set new opened file's FD to: %d\n", fd2);
    } else  {
        printf("*** You incorrectly set new opened file's FD to: %d\n", fd2);
    }

    shutdown();
    return 0;
}
