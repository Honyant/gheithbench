#include "libc.h"

int main(int argc, char** argv) {
    // This is a fairly straight forward test case.
    // I am attempting to execl several invalid executables.
    // All of them can be found to be invalid via the data you can see in elf load.
    // Every one of them should fail, and you should be able to continue executing this program
    // after a failed execl (don't free all the memory before you know if the execl will succeed!).
    printf("*** Welcome to the bad executable test case!\n");
    printf("*** This is a fairly straight forward test case.\n");
    printf("*** All of them can be found to be invalid via the data you can see in elf load.\n");
    printf("*** Every one of them should fail, and you should be able to continue executing this program.\n");
    printf("*** after a failed execl (don't free all the memory before you know if the execl will succeed!)\n");

    printf("*** Part 0: attempting to execl a nonexistent file\n");
    printf("*** You can tell this is invalid because the file does not exist\n");
    uint32_t execlReturn = execl("/sbin/thisFileDoesNotExist", "thisArgShouldNeverBeRead",0);

    printf("*** did execl return -1? %d\n",(execlReturn == -1));
    
    printf("*** Part 1: attempting to execl a tiny text file\n");
    printf("*** You can tell this is invalid because it is too small to contian an elf header\n");
    execlReturn = execl("/sbin/literallyJustATextFile", "thisArgShouldNeverBeRead",0);

    printf("*** did execl return -1? %d\n",(execlReturn == -1));

    printf("*** Part 2: attempting to execl a BIG text file\n");
    printf("*** You can tell this is invalid because the elf header does not contain the correct magic bytes (explained in elf.h)\n");
    execlReturn = execl("/sbin/aVeryLargeTextFile", "thisArgShouldNeverBeRead",0);

    printf("*** did execl return -1? %d\n",(execlReturn == -1));

    printf("*** Part 3: attempting to execl an invalid load address (trying to load the program into kernel memory)\n");
    printf("*** You can tell this is invalid because it asks to be loaded at a kernel address\n");
    execlReturn = execl("/sbin/invalidLoadAddress", "thisArgShouldNeverBeRead",0);

    printf("*** did execl return -1? %d\n",(execlReturn == -1));

    printf("*** Great Job! You did it!\n");
    
    shutdown();
    return 0;
}
