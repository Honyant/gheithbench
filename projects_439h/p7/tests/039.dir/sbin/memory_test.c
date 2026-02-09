#include "stdint.h"
#include "libc.h"

void try_to_access_kernel_memory() {
    // processes that run this function should be killed. 
    uint32_t kernel_start = 0x00001000;
    uint32_t kernel_end = 0x80000000;
    uint32_t step = 0x00010000; // speeds up the loop
    for (uint32_t i = kernel_start; i < kernel_end; i += step){
        ((uint32_t*)i)[0] = 0;
        printf("*** a process wrote to kernel memory...");
    }
}

void try_to_sneak_into_kernel_memory() {
    // processes that run this function should be killed. 
    uintptr_t user_start = 0x80000000;
    uint32_t step = 0x00010000;
    for (uint32_t i = step; i < 0x80000000; i += step) {
        ((uint32_t*)(user_start-i))[0] = 0;
        printf("*** a process snuck into and wrote to kernel memory...\n");
    }
}

int main(int argc, char** argv){
    int id = fork();
    if (id < 0) printf("*** fork failed in memory test\n");
    else if (id == 0) try_to_access_kernel_memory();
    else try_to_sneak_into_kernel_memory();
    uint32_t status = 42;
    wait(id, &status);
    return 0;
}