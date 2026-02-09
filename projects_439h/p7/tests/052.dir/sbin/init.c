#include "libc.h"

// This test case will try to attack the kernel by writing to invalid places in memory. Make
// sure to validate all buffers and pointers in memory so that the kernel doesn't hurt itself.

int main(int argc, char** argv) {

    printf("*** Start of tc\n");

    int result, fd;

    // Test writing to kernel memory using a bad buffer through read
    for (uint32_t i = 0x90; i < 0x100; i ++) {
        int fd = open("/etc/data.txt",0);
        int result = read(fd, (void *) (i << 4), len(fd));
        if (result >= 0) {
            printf("*** Failed to protect read from bad buffer pointers\n");
            return -1;
        }
        close(fd);
    }

    fd = open("/etc/data.txt",0);
    result = read(fd, (void *) 0xFFFFFFFF, len(fd));
    if (result >= 0) {
        printf("*** Failed to protect read from bad buffer pointers (overflow)");
    }
    close(fd);

    fd = open("/etc/data.txt",0);
    result = read(fd, (void *) 0x400000, len(fd));
    if (result >= 0) {
        printf("*** Failed to protect read from bad buffer pointers part 2");
    }
    close(fd);

    printf("*** Past Read Tests\n");


    // Test printing out kernel memory using write
    result = write(1, (void*) 0x1000, 0x1000000 - 0x1000);
    if (result >= 0) {
        printf("*** Failed to protect write from bad buffer pointers\n");
        return -1;
    }

    result = write(1, (void*) 0xFFFFFFFF, 0x10000);
    if (result >= 0) {
        printf("*** Failed to protect write from bad buffer pointers (overflow)\n");
        return -1;
    }

    result = write(1, (void*) 0xFFFFFFF, 0x8000000F);
    if (result >= 0) {
        printf("*** Failed to protect write from bad buffer pointers (more overflow)\n");
        return -1;
    }

    printf("*** Past Write Tests\n");

    // Testing that there are bounds checks on the descriptor ids

    result = close(-1);
    if (result >= 0) printf("** Failed to check bounds on close syscall!!\n");
    result = close(301);
    if (result >= 0) printf("** Failed to check bounds on close syscall!!\n");
    result = close(-10);
    if (result >= 0) printf("** Failed to check bounds on close syscall!!\n");
    result = close(-15);
    if (result >= 0) printf("** Failed to check bounds on close syscall!!\n");
    result = close(305);
    if (result >= 0) printf("** Failed to check bounds on close syscall!!\n");
    result = close(310);
    if (result >= 0) printf("** Failed to check bounds on close syscall!!\n");

    printf("*** Past Close section\n");

    // This should not be allowed to happen. Your thread should kill the process here, because it will try to write
    // a stack frame in kernel memory.
    int pid = fork();
    if (pid == 0) {  
        uint32_t* ptr = (uint32_t*) 0x100000;
        // This should cause the child to exit
        *ptr = 5;
        printf("*** should not be printing at this point\n");
        shutdown();
    } else if (pid > 0){
        printf("*** Passed the test case\n");
        shutdown();
    } else {
        printf("*** Fork should not fail here\n");
        shutdown();
    }
    return 0;
}
