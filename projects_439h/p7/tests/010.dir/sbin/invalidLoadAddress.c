#include "libc.h"

int main(int argc, char** argv) {
    printf("*** uh oh! you shouldn't have loaded the elf file that had an invalid addr\n");
    shutdown();
    exit(-1);    
    return 0;
}
