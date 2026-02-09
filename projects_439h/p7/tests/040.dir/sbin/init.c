#include "libc.h"

void fill_stack() {
    char large_data[1024 * 1024 * 64]; // 64MB array to fill a significant portion of the stack
    for (int i = 0; i < sizeof(large_data); i++) {
        large_data[i] = (char)(i % 256);
    }
    printf("*** Filled 64MB of stack space\n");

    char more_data[1024 * 1024 * 60]; // Another 64MB array to fully use stack space up to 128MB
    for (int i = 0; i < sizeof(more_data); i++) {
        more_data[i] = (char)((i + 128) % 256);
    }
    printf("*** Filled additional 60MB of stack space\n");
}

int main(int argc, char** argv) {
    printf("*** Starting execl stack test\n");

    // Step 1: Fill up the stack space
    fill_stack();

    // Step 2: Attempt to execl a new process (shell.c)
    printf("*** Attempting to execl to shell\n");
    int rc = execl("/sbin/shell", "shell", "arg1", "arg2", "arg3", 0);

    // Step 3: Check if execl failed
    printf("*** execl failed, rc = %d\n", rc);

    // Shutdown as a fail-safe
    shutdown();
    return -1;
}
