#include "libc.h"

void one(int fd) {
    printf("*** fd = %d\n",fd);
    printf("*** len = %d\n",len(fd));

    cp(fd,2);
}

int main(int argc, char** argv) {
    printf("*** %d\n",argc);
    for (int i=0; i<argc; i++) {
        printf("*** %s\n",argv[i]);
    }
    
    // This test has some modifications to 
    // t0 to make it easier to understand
    // and check for edge cases

    // In particular, this test focuses on seek

    // we should not be able to seek on stdin
    if (seek(0, 1) >= 0) printf("*** Failed seek test\n");

    close(0); // close stdin
    int fd = open("/etc/data.txt", 0); // this should go to fd 0 now
    one(fd); // and we should be able to read from it

    // now we should be able to seek on 0 because stdin is closed
    if (seek(0, 1) < 0) printf("*** Failed seek test\n");

    // Make sure that seek works
    for (uint32_t i = 0; i < 5; i++) {
        seek(0, 0);
        one(fd);
    }

    // Now do it in the middle of the file
    for (uint32_t i = 0; i < 5; i++) {
        seek(0, 17);
        one(fd);
    }

    // Seek past the file
    seek(0, 0xFFFFFFFF);
    one(fd); // this should fail
    seek(0, 0);
    one(fd); // and this should be fine

    int id = fork();

    if (id < 0) {
        printf("fork failed");
    } else if (id == 0) {
        /* child */
        printf("*** in child\n");

        ///////////////////////////////
        // TEST                      //
        // MAKE SURE WE CAN PASS     //
        // ARGUMENTS WITH LENGTH > 1 //
        ///////////////////////////////

        int rc = execl("/sbin/shell","shell","1234567890","abcd","efgh",0);
        printf("*** execl failed, rc = %d\n",rc);
    } else {
        /* parent */
        uint32_t status = 42;
        wait(id,&status);
        printf("*** back from wait %ld\n",status);

        int fd = open("/etc/panic.txt",0);
        cp(fd,1);
    }

    shutdown();
    return 0;
}
