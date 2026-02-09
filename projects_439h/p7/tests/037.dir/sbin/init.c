#include "libc.h"

void one(int fd) {
    printf("*** fd = %d\n",fd);
    printf("*** len = %d\n",len(fd));

    cp(fd,2);
}

int main(int argc, char** argv) {

    //this test case starts the text segment near the very end of private virtual memory
    //if you kept Gheith's method of always putting the stack at the end, you might run into problems
    //otherwise, yippee :)


    printf("*** congrats on making it to main!\n");

    //keeping this from t0 bc why not, nice little starting point
    //sanity checks that it's good
    //because apparently sometimes it's not. I get argc very wrong ):
    printf("*** %d\n",argc);
    for (int i=0; i<argc; i++) {
        printf("*** %s\n",argv[i]);
    }

    //a little seek edge case: shouldn't be able to seek on i/o devices
    //so seek on stdin stdout or stderr shouldn't work

    int s = seek(0, 10);

    if (s >= 0) {
        printf("*** oops! shouldn't succeed on seeking for stdin\n");
        shutdown();
    }

    s = seek(1, 15);

    if (s >= 0) {
        printf("*** oops! shouldn't succeed on seeking for stdout\n");
        shutdown();
    }

    s = seek(2, 20);

    if (s >= 0) {
        printf("*** oops! shouldn't succeed on seeking for stderr\n");
        shutdown();
    }

    printf("*** good job! you don't seek on stdin/out/err\n");

    int c = close(2);

    if (c != 0) {
        printf("*** failed to close stderr ):\n");
        shutdown();
    }

    printf("*** closed stderr: close = %d\n", c);

    int hello = open("hello", -0x4e110); //since flags is ignored anyways

    if (hello != 2) {
        printf("*** supposed to give lowest open fd (2)\n");
        shutdown();
    }

    printf("*** hooray! opened hello at %d\n", hello);

    s = seek(2, 15); //this time it should work

    if (s != 15) {
        printf("*** seek should work now! sterr has been replaced with a normal file\n");
        shutdown();
    }

    s = seek(2, 0); //set it back to beginning
    ssize_t l = len(2); //get the length

    if (l != 13) {
        printf("*** length should be 13?\n");
        shutdown();
    }

    char buf[l + 1];
    buf[l] = 0;

    //needa read the whole file
    int r = read(2, buf, l);
    while (r != l) {
        int n = read(2, buf, l-r);
        r += n;
        if (n == 0) {
            //if n == 0, r != l and did not = l
            printf("*** failed to read at all ):\n");
        }
    }

    //printing the file
    //file contains *** and newline
    printf("%s", buf);

    printf("*** good job! bye bye\n");

    shutdown();
    return 0;
}
