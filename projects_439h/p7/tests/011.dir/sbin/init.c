#include "libc.h"
#include "debug.h"

uint32_t data[0x1000]; // will be placed in BSS segment

int main(int argc, char** argv) {
    printf("*** Test started.\n");
    
    // harvest approximate esp and eip
    uint32_t eip = (uint32_t) &printf; // address of function
    uint32_t esp = (uint32_t) &eip; // address of local variable
    printf("*** Stack pointer and program counter obtained.\n");

    // you shouldn't actually unmap these: if you do, you'll crash
    naive_unmap((void*) esp); // in stack segment
    printf("*** Stack segment \"unmapped\".\n");
    naive_unmap((void*) eip); // in text segment
    printf("*** Text segment \"unmapped\".\n");
    naive_unmap((void*) data); // in BSS segment
    printf("*** BSS segment \"unmapped\".\n");

    // do some work
    for (int i = 0; i < 0x1000; i++) {
        data[i] = i * i; // uses BSS
    }
    uint32_t sum = 0; // stack variable
    for (int i = 0; i < 0x1000; i++) {
        sum += data[i]; // uses BSS and stack
    }
    printf("*** Computed value: 0x%lx\n", sum);

    printf("*** You passed!\n");
    shutdown();
    ASSERT(0); // lol
}
