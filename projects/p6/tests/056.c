#include <stdio.h>

#include "src/go.h"

Channel* yup;

Value yop() {
    long r = receive(yup).asLong;
    printf("recieved %ld, yopyopyop\n", r);
    return asInt(0);
}

Value yap() {
    send(yup, (Value) 55);
    printf("sent %d, yapyapyap\n", 55);
    return asInt(0);
}

int main() {
    go(yap);
    go(yop);
    yup = channel();

    for (int i = 0; i < 4; i++) {
        if (i % 2 == 0) { // even
            yop();
        } else { // odd
            yap();
        }
    }

    printf("this shouldn't be the last line\n");
    receive(yup);
    printf("you should see this\n");

    return 0;
}