#include "elf.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("usage: %s <ARM executable file name>\n",argv[0]);
        exit(-1);
    }

    uint64_t entry = loadElf(argv[1]);
    printf("entry = %llx\n",(long long)entry);
    return 0;
}
