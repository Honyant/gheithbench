#include "libc.h"

/*
    This test aims to ensure values are correctly shared/copied as new processes are created. 
    Here are the tests:
        1. Memory Test: Can processess access kernel memory? 
        2. File Test: Are file directories correctly shared/copied between processes?
        3. VME Test: Are VMEs correctly shared/copied between processes?
    
    In this file, I use fork and execl to reach my other tests. Please see those files. The 
    code is updated and matches the elf file--I promise :)
*/

int main(int argc, char** argv) {
    int id1 = fork();
    if (id1 < 0) printf("*** fork failed\n");
    else if (id1 == 0) execl("/sbin/memory_test", 0);
    else {
        uint32_t status = 42;
        wait(id1, &status);
        printf("*** finished memory test\n");
    }

    int id2 = fork();
    if (id2 < 0) printf("*** fork failed\n");
    else if (id2 == 0) execl("/sbin/file_test", 0);
    else {
        uint32_t status = 42;
        wait(id2, &status);
        printf("*** finished file test\n");
    }

    int id3 = fork();
    if (id3 < 0) printf("*** fork failed\n");
    else if (id3 == 0) execl("sbin/vme_test", 0);
    else {
        uint32_t status = 42; 
        wait(id3, &status);
        printf("*** finished vme test\n");
    }

    shutdown();
    printf("*** shutdown returned...not good!\n");
    return 0;
}