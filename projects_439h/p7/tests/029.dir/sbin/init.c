#include "libc.h"

// File shenaniganery test case
// Basic write, file opening, and closing
// Relative chdir
// Closing stdout and then attempting a write (should fail)

int main(int argc, char** argv) {
    printf("*** %d\n",argc); 

    // A relative chdir test
    int changed = chdir("/tate/");

    printf((changed == 0) ? "*** Yay you successfully changed directories\n" : "*** Gasp you failed changing directories\n");

    // Now can you actually read given the changed working directory?
    int fd = open("tatertot/itsokimok.txt",0);

    printf("*** fd = %d\n",fd);
    printf("*** len = %d\n",len(fd));

    cp(fd,2);

    // Now close the file, and read attempt
    close(fd);

    cp(fd, 2); // Should fail

    // Now what happens when we close stdout and stderr?
    close(1);
    close(2);


    printf("*** Closed stdout, this should not print\n");

    shutdown();
    return 0;
}
