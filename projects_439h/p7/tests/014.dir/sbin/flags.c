#include "lib/libc.h"

// This file isn't used anymore... it's for the execl flags test

// In crt0.S, the flags register is pushed onto the stack underneath argc and argv
// We can then use it as a parameter to main (the compiler doesn't like it though)
int main(int argc, const char** argv, uint32_t flags) {
    int oFlag = flags & 0x0001; // Is the overflow flag set?
    printf("***     After execl: overflow flag is %s\n", oFlag ? "SET" : "UNSET");

    return 100;
}