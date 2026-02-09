#include "libc.h"

void one(int fd) {
    printf("*** fd = %d\n",fd);
    printf("*** len = %d\n",len(fd));

    cp(fd,2);
}

/*
The first half is taken from t0, as a quick sanity check.
The second half is a test to make sure that accessing kernel memory from a user process leads to the process exiting with the correct status code. 
*/

int main(int argc, char** argv) {
    printf("*** %d\n",argc);
    for (int i=0; i<argc; i++) {
        printf("*** %s\n",argv[i]);
    }
    int fd = open("/etc/data.txt",0);
    one(fd);

    printf("*** close = %d\n",close(fd));

    one(fd);
    one(100);


    printf("*** open again %d\n",open("/etc/data.txt",0));
    printf("*** seek %ld\n",seek(3,17));
    
    int id = fork();
    // printf("ID: %d\n", id);

    if (id < 0) {
        printf("*** fork failed");
    } else if (id == 0) {
        /* child */
        printf("*** in child\n");
        int* some_integer = (int*)0x00200000; // accessing kernel memory
        int access = *some_integer;
        printf("*** This should not be printed, obtained %d\n", access);
        exit(0);
    } else {
        /* parent */
        uint32_t status = 42; // the answer to life the universe and everything
        wait(id,&status);
        printf("*** back from wait %ld\n",status);

        int fd = open("/etc/panic.txt",0);
        cp(fd,1);
    }

    shutdown();
    return 0;
}
