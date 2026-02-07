#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "src/go.h"

int counter = 0;

/*
In this testcase, I test that returning is handled correctly, and that there are infinite sends after a c-routine returns

Futhermore, I check that again() works as followed and that you are not creating additional Routines when calling again()
*/

Value test_channel() {
    for (int i = 0; i < 3; i++)
        send(me(), asU64(i));

    return asU64(12374);
}

Value again_test() {
    Channel *ch = go(test_channel);

    for (int i = 0; i < 3; i++) {
        uint64_t rec = receive(ch).asU64;
        if (rec != i) {
            printf("%s",  "You are receiving wrong values!\n");
        }
    }

    for (int i = 0; i < 2; i++) {
        uint64_t ret = receive(ch).asU64;
        if (ret != 12374) {
            printf("%s", "You are not handling return correctly\n");
        }
    }

    return asU64(123);
}

int main() {
    again_test();

    if (counter < 250000) {
        counter++;
        again();
    }

    printf("%s", "You passed!\n");

    return 0;
}