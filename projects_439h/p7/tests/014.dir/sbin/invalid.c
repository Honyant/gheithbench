#include "lib/libc.h"

int main() {
    printf("*** This should not run...\n");
    shutdown();
}