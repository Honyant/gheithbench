#include "libc.h"

int main(int argc, char** argv) {

    //this writes the alphabet into a buffer and then prints it to standard error via the write syscall
    //tests that fd 2 is directed to console output by the first process
    char* buffer = malloc(sizeof(char)*27);
    for (int i = 0; i < 26; i++) {
        buffer[i] = (char) (65 + i);
    }
    buffer[26] = '\n';

    printf("*** ");

    for (int i = 0; i < 27; i++) {
        write(2, buffer+i, 1);
    }

    //tests that the lowest available file descriptor is assigned to opened files
    int id1 = open("/info/file.txt", 0);
    printf("*** Assigned File Descriptor: %d\n", id1);

    //tests that close works as intended
    //file write should work as expected before file is closed:
    printf("*** ");
    int response1 = write(2, buffer, 1);
    printf("\n");
    printf("*** Response before close: %d\n", response1);

    //after the file is closed, any writes to it should error:
    close(2);

    int response2 = write(2, buffer, 1);

    printf("*** Response after close is correct: %d\n", response2<0 ? 1 : 0);

    //tests that the lowest available file descriptor is assigned to open files (we now have space at fd=2)
    int id2 = open("/info/file2.txt", 0);
    printf("*** Assigned File Descriptor: %d\n", id2);

    //tests that we can close the standard out/err file descriptors, open a new file there, and read from that file
    //reusing file descriptor 2 for a file (not standard error)
    char* reassigned_file = malloc(sizeof(char)*1);
    printf("*** ");
    for (int i = 0; i < 12; i++) {
        read(2, reassigned_file, 1);
        printf("%s", reassigned_file);
    }

    printf("\n");

    //test that offsets are shared between parent and child processes
    //tests that the read syscall shifts the offset to read
    char* buffer2 = malloc(sizeof(char)*1);
    printf("*** ");

    //we perform 19 reads in the parent process
    for (int i = 0; i < 19; i++) {
        read(3, buffer2, 1);
        printf("%s", buffer2);
    }
    printf("\n");

    //we then fork. The child process should inherit the file descriptors and offsets.
    int process_id = fork();

    if (process_id == 0) {
        //A read into this file should start at the offset that the parent process left off at.
        printf("*** ");
        read(3, buffer2, 1);
        printf("%s", buffer2);
        printf("\n");

        //child processes should be able to fork and create child processes
        //tests that the wait() syscall currently handles nested child processes that the parents are waiting on
        int process_id_inner = fork();

        if (process_id_inner == 0) {
            //The same file descriptor is inherited from the grandparent process to the parent process for this nested process
            //the offset that was incremented by both the grandparent and parent process needs to be reflected on the next read
            //the next read should be at the offset after that of the grandparent process + parent process
            printf("*** ");
            read(3, buffer2, 1);
            printf("%s", buffer2);
            printf("*** Process inside a process!\n");

            //tests that the exit syscall correctly resolves nested processes
            exit(110);
        } else {
            uint32_t response = 1;
            wait(process_id_inner, &response);
            printf("*** Response from inner process: %ld\n", response);
        }
        //tests that the exit syscall correctly resolves nested processes
        exit(55);
    } else {
        uint32_t in = 1;
        wait(process_id, &in);
        printf("*** %ld\n", in);
    }

    //testing some edge cases that should not crash the kernel
    //reading a file that has been closed should respond with negative value
    close(3);
    char* buffer3 = malloc(sizeof(char)*1);
    int invalid_read_response = read(3, buffer3, 1);
    if (invalid_read_response < 0) {
        printf("*** Correctly handled invalid read of closed file\n");
    }

    //closing a file that has already been closed should respond with a negative value
    int invalid_close_response = close(3);
    if (invalid_close_response < 0) {
        printf("*** Correctly handled closing already closed file.\n");
    }
    
    shutdown();

    return 0;
}
