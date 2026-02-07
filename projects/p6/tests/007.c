#include <stdbool.h>
#include <stdio.h>

#include "src/go.h"

int mainCount = 0;
int fCount = 0;

Value f() {
    printf("fCount: %d\n", fCount++);
    if (fCount < 5) {
        again();
    }
    return asInt(42);
}

int main() {
    // Test again on main
    printf("mainCount: %d\n", mainCount++);
    if (mainCount < 10) {
        again();
    }

    // Test again on non-main function
    Channel *ch = go(f);
    receive(ch);

    // Test that return sends infinitely
    bool passed = true;
    for (int i = 0; i < 100000; i++) {
        if (receive(ch).asInt != 42) {
            printf("(Fail) Return statement does not infinitely send its return value.");
            passed = false;
        }
    }

    if (passed) {
        printf("(Pass) Return statement infinitely sends its return value.");
    }

    return 0;
}
