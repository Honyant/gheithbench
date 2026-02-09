#include "libc.h"

int main(int argc, char** argv) {
    uint32_t* count = (uint32_t*) 0xF0000000;
    if (count[0] == 500){
        printf("*** all execls done\n");
        shutdown();
        return 666;
    }

    count[0]++;
    int rc = execl("/sbin/shell","shell", 0);
    printf("*** execl failed on count: %lx, rc = %d\n", count[0], rc);
    return 666;
}
