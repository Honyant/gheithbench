#include "libc.h"

void one(int fd) {
    printf("*** fd = %d\n",fd);
    printf("*** len = %d\n",len(fd));

    cp(fd,2);
}

// this test case focuses on testing basic functionality and edge cases with files, chdir, fork, and seek

int main(int argc, char** argv) {

    int fd = open("/data/welcome.txt",0);
    one(fd);

    printf("*** close = %d\n",close(fd));

    // test opening a file that doesn't exist
    int error = open("doesnotexist",0);
    if (error < 0) {
        printf("*** cinnamoroll is lost\n");
    }

    // test seek on console device returns error
    int error2 = seek(2, 10);
    if (error2 < 0) {
        printf("*** kuromi is lost\n");
    }

    // test read on console device
    char buf[10];
    int error3 = read(1, buf, 10);
    if (error3 < 0) {
        printf("*** pochaco is lost\n");
    }

    // test write on stdin
    int error4 = write(0, buf, 10);
    if (error4 < 0) {
        printf("*** my melody is lost\n");
    }

    int random = 1234;

    int fd2 = open("/data/character_list.txt",0);
    
    // test fork
    int id = fork();

    if (id < 0) {
        printf("fork failed");
    } else if (id == 0) {
        /* child */
        printf("*** in child\n");

        // check that the child's stack is preserved
        printf("*** %d\n", random);

        // test seek
        printf("*** seek %ld\n",seek(fd2,13));
        one(fd2);

        exit(1);
    } else {
        /* parent */
        uint32_t status = 42;
        wait(id,&status);
        printf("*** back from wait %ld\n",status);

        // test chdir
        printf("*** chdir: %d\n", chdir("data/data2"));
        int fd3 = open("cinnamoroll.txt",0);
        one(fd3);
        close(fd3);

        // test chdir with / at the end and beginning
        printf("*** chdir: %d\n", chdir("/data/data2/"));
        int fd4 = open("cinnamoroll.txt",0);
        one(fd4);
        close(fd4);

        // test chdir with an invalid path
        printf("*** chdir: %d\n", chdir("/data/data3/"));
    }

    shutdown();
    return 0;
}
