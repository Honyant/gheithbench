#include "libc.h"


//this test case will just have a process that executes itself a bunch of times to test that you clean up things properly when calling exec
//init sets up some shared memory and then execls shell which will execl itself a lot

int main(int argc, char** argv) {

    //map a non existent file to shared memory so we have some page in shared memory that we can modify
    uint32_t* count = (uint32_t*) naive_mmap(4, 1, 5, 0);
    if (count != (uint32_t*) 0xF0000000){
        printf("*** mmap failed\n");
        shutdown();
    }
    count[0]=0;


    int rc = execl("/sbin/shell","shell", 0);
    printf("*** execl failed, rc = %d\n",rc);

    shutdown();
    return 0;
}
