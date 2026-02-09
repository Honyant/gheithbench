#include "debug.h"
#include "threads.h"
#include "atomic.h"

const int N = 1000;

// Test freeing TCBs after they are done
void kernelMain(void) {
    for (int i = 0; i < N; i++) {
        thread([] {});
    }
    Debug::printf("*** done\n");
}

