#include "libc.h"



int main(int argc, char** argv) {

    /*
    fork/wait/exit test!
    */
    int f1 = fork();
    int f2 = fork();
    int f3 = fork();
    int f4 = fork();

    int out = 1;
    uint32_t status;
    if (f4 > 0) {
        wait(f4, &status); // status should be 1
        out += status; // out becomes 2
    }
    else {
        exit(out); // exit(1)
    }
    if (f3 > 0) {
        wait(f3, &status); // status should be 2
        out += status; // out becomes 4
    }
    else {
        exit(out); // exit(2)
    }
    if (f2 > 0) {
        wait(f2, &status); // status should be 4
        out += status; // out becomes 8
    }
    else {
        exit(out); // exit(4)
    }
    if (f1 > 0) {
        wait(f1, &status); // status should be 8
        out += status; // out becomes 16
    }
    else {
        exit(out); // exit(8)
    }

    printf("*** # of processes after 4 forks: %d\n", out);

    /*
    This test makes sure kernel memory remains inaccessible to the user!

    Users may attempt to use your syscalls to access memory they shouldn't be able to see, make 
    sure you don't let them!
    */
    printf("*** Printing MBR (this shouldn't be allowed): \n*** ");
    int totalWritten = 0;
    int written = 0;
    int ptr = 0x7c00; // MBR start
    while (written != -1) {
        totalWritten += written;
        ptr += written;
        written = write(1, (void*)ptr, 512 - totalWritten);
    }
    printf("\n*** Done MBR, read %d bytes\n", totalWritten);



    shutdown();
    return 0;
}
