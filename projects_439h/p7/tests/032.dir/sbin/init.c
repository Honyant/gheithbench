#include "libc.h"

// Simple test to check that stdout and stderr don't print after being closed

int main(int argc, char** argv) {
    write(1, "*** should print\n", 17);
    write(2, "*** should print\n", 17);

    close(1);
    close(2);

    write(1, "*** shouldn't print\n", 20);
    write(2, "*** shouldn't print\n", 20);

    shutdown();
    return 0;
}
