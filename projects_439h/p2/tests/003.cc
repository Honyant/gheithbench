#include "atomic.h"
#include "barrier.h"
#include "debug.h"
#include "promise.h"
#include "random.h"
#include "threads.h"

Atomic<uint32_t> thread_step{0};
Atomic<uint32_t> main_step{0};
const uint32_t SIZE = 200;
uint32_t mat[SIZE][SIZE];
Random random{42};

void fill_arr() {
    Debug::printf("*** Filling array\n");
    for (uint32_t i = 0; i < SIZE; i++) {
        for (uint32_t j = 0; j < SIZE; j++) {
            mat[i][j] = random.next() % 100;
        }
    }
    Debug::printf("*** Filled array\n");
}

void print_arr() {
    Debug::printf("*** Printing array\n");
    for (uint32_t i = 0; i < SIZE; i++) {
        Debug::printf("*** ");
        for (uint32_t j = 0; j < SIZE; j++) {
            Debug::printf("%d ", mat[i][j]);
        }
        Debug::printf("\n");
    }
    Debug::printf("*** Printed array\n");
}

void kernelMain(void) {
    Debug::printf("*** Kernel main\n");
    auto p = new Promise<int>();
    auto p1 = new Promise<int>();

    thread([p] {
        fill_arr();
        p->set(0);
    });
    thread([p, p1] {
        p->get();
        print_arr();
        p1->set(0);
    });   

    p1->get();
    
    Debug::printf("*** Kernel main end\n");
    Debug::shutdown();
}
