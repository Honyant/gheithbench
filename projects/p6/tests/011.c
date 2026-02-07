#include <stdio.h>

#include "src/go.h"

Value f1() {
    return asLong(100);
}

Value f2() {
    printf("%s", receive(me()).asString);
    return asLong(0);
}

Value f3() {
    send(me(), asString("hello"));
    again();
    return asInt(0);
}

Value f4() {
    send(me(), asInt(1));
    again();
    return asInt(0);
}

int main(int argc, char *argv[]) {
    // test infinite return functionality
    Channel* c1 = go(f1);
    long x = 0;
    for (int i = 0; i < 100; i++) {
        x += receive(c1).asLong;
    }
    printf("%ld\n", x);

    // test send and receive basic
    Channel* c2 = go(f2);
    send(c2, asString(":) "));

    // test again
    Channel* c3 = go(f3);
    Value v1 = receive(c3);
    Value v2 = receive(c3);
    printf("\nv1: %s\n", v1.asString);
    printf("v2: %s\n", v2.asString);

    // test send and receive in loops
    Channel* c4 = go(f4);
    int count = 0;
    int y = 0;
    while (count < 5) {
        count++;
        y += receive(c4).asInt;
    }
    if (count == y) {
        printf("success\n");
    }

    // test blocked routine
    Channel* c5 = channel();
    send(c5, asString("stinky"));
    printf("this should never print\n");
    return 0;
}