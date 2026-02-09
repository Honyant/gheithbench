#include "libc.h"

void one(int fd) {
    printf("*** fd = %d\n",fd);
    printf("*** len = %d\n",len(fd));

    cp(fd,2);
}

/**
 * This test case hopefully checks a lot of failure cases :)
 */
int main(int argc, char** argv) {
    printf("*** Welcome to my test case!\n");

    /**
     * Basic file tests!
     */
    printf("*** Basic file tests start\n");

    // opening a file that exists
    int fd = open("file1", 0);
    one(fd);
    if (fd != 3) {
        printf("*** The lowest file descriptor wasn't given to the file descriptor\n");
    }

    close(fd);
    one(fd);

    // opening a file that doesn't exist
    fd = open("thisDoesntExist", 0);
    if (fd >= 0) {
        printf("*** Allocating space for a file that doesn't exist\n");
    }
    one(fd); // fd is -1 now

    // opening a file using a path
    fd = open("folder1/folder11/folder111/file1111", 0);
    if (fd != 3) {
        printf("*** The lowest file descriptor wasn't given to the file descriptor\n");
    }
    one(fd);
    close(fd);

    printf("*** Basic file tests end\n");

    /**
     * Chdir tests :)
     */
    printf("*** Chdir tests start\n");

    // testing chdir where the folder directly exists in the parent 
    int res = chdir("folder1/");
    fd = open("file11", 0);
    if (fd != 3) {
        printf("*** The lowest file descriptor wasn't given to the file descriptor\n");
    }
    if (res != 0) {
        printf("*** Returning a non zero value for a success\n");
    }
    one(fd);
    close(fd);

    // testing chdir where the folder does not directly exist in the parent (path, not name)
    res = chdir("folder11/folder111/");
    fd = open("file1111", 0);
    if (fd != 3) {
        printf("*** The lowest file descriptor wasn't given to the file descriptor\n");
    }
    if (res != 0) {
        printf("*** Returning a non zero value for a success\n");
    }
    one(fd);
    close(fd);

    // testing chdir with an absolute path
    res = chdir("/folder1/");
    fd = open("file11", 0);
    if (fd != 3) {
        printf("*** The lowest file descriptor wasn't given to the file descriptor\n");
    }
    if (res != 0) {
        printf("*** Returning a non zero value for a success\n");
    }
    one(fd);
    close(fd);

    // testing chdir where the path does not exist
    res = chdir("/thisDoesNotExist/");
    if (res < 0) {
        printf("*** Correctly failed on changing to a nonexistent directory\n");
    }

    chdir("/"); // changes back to root so rest of test can run normally
    printf("*** Chdir tests end\n");

    /**
     * Fork test!
     */
    printf("*** Fork test starts\n");
    int child = fork();
    if (child < 0) {
        printf("*** Fork failed\n");
    } else if (child == 0) {
        printf("*** In child!\n");
        exit(0);
    } else {
        uint32_t status = 42;
        wait(child, &status);
        printf("*** Back in parent\n");
    }
    printf("*** Fork test ends\n");

    /**
     * Writing tests!
     */
    printf("*** Starting writing tests\n");
    // opening a file so that it's an actual file that we can fail at writing to
    int failFile = open("file2", 0);
    one(failFile);

    // writing to something that cannot be written to in a read only system (failFile)
    int writeToRead = write(failFile, "hii", 3);
    if (writeToRead < 0) {
        printf("*** Yay successfully didn't write\n");
    } else {
        printf("*** Returned value greater than 0 – returned: %d\n", writeToRead);
    }

    // Explicitly writing with std::err (fd 2)
    write(2, "*** hello\n", 10);

    printf("*** This should be the last thing that prints normally!\n");
    // Closing std::out and std::err
    close(1);
    close(2);
    printf("*** Don't print this\n");
    write(2, "*** hello\n", 10);

    shutdown();
    return 0;
}
