#include "libc.h"

int main(int argc, char** argv) {
    printf("*** This should not have run! Here is your failure point: %s\n", argv[0]);
    shutdown();
    return 0;
}
