#include "libc.h"

void checking(int fd, int fd_, int len_) {
    if (fd != fd_) {
        printf("*** || Failed Test: got %d but expected %d for fd\n", fd, fd_);
    }
    if (len(fd) != len_) {
        printf("*** || Failed Test: got %d but expected %d for fd\n", len(fd), len_);
    }
}

int main(int argc, char** argv) {
    printf("*** Starting Test ***\n");

    printf("\n*** Basic File Test ***\n");
    int fd = open("/data/data.txt",0); 
    
    checking(fd, 3, 12);
    cp(fd,2);

    printf("\n*** Basic Close File Test ***\n");
    close(fd);
    checking(fd, 3, -1);

    int status = close(123);
    if (status != -1) {
        printf("*** || Failed Close Test: got %d but expected %d for fd\n", status, -1);
    }

    printf("\n*** Basic Fork Test ***\n");

    int fork_id = fork();
    if (fork_id == 0) {
        // child process
        printf("*** baby duck\n");
        exit(0);
    } else if (fork_id > 0) {
        // parent process
        uint32_t status = 123;
        wait(fork_id, &status);
        if (status == 123 || (int) status < 0) {
            printf("*** || Failed Fork Test: got status %ld\n", status);
        }
        printf("*** Found a duckling!\n");
        close(fork_id);
    } else {
        printf("*** || Failed Fork Test: got id %d\n", fork_id);
    }

    printf("\n*** Fork Close Test ***\n");

    fork_id = fork();
    if (fork_id == 0) {
        // child process
        close(1); // Closing std::out
        printf("*** Goose?!?! There shouldn't be a goose here\n");
        int fd1= open("/data/data.txt",0);
        checking(fd1, 1, 12);
        exit(0);
    } else if (fork_id > 0) {
        // parent process
        uint32_t status = 123;
        wait(fork_id, &status);
        if (status == 123 || (int) status < 0) {
            printf("*** || Failed Fork Test: got status %ld\n", status);
        }
        printf("*** no duckling?\n");
        close(fork_id);
    } else {
        printf("*** || Failed Fork Test: got id %d\n", fork_id);
    }

    printf("\n*** Basic Read Test ***\n");

    char c = 0;

    int fd2 = open("/data/text.txt",0); 
    for (int i = 0; i < len(fd2); i++) {
        status = read(fd2, &c, 1);

        if (status != 1) { printf("*** || Failed Read Test: Received status %d\n", status); }

        status = write(1, &c, 1);
        if (status != 1) { printf("*** || Failed Write Test: Received status %d\n", status); }
    }

    printf("\n*** all done\n");

    shutdown();
    return 0;
}
