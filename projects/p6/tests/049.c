#include <stdio.h>

#include "src/go.h"

// THIS TEST JUST TESTS SOME SIMPLE SENDING AND RECEIVING

Value fun1() {
    Value v = receive(me());
    printf("%s", v.asString);
    return asString("Test 1 done\n");
}

Value fun2() {
    for (int i = 0; i < 10; i++) {
        Value v = receive(me());
        printf("%d ", v.asInt);
    }
    return asString("\nTest 2 done\n");
}

Value fun3() {
    for (int i = 0; i < 11; i++) {
        send(me(), asInt(i));
    }
    return asString("\nTest 3 done\n");
}

Value fun4() {
    for (int i = 0; i < 2; i++) {
        Value v = receive(me());
        printf("%d\n", v.asInt);
    }
    return asString("There is an infinite supply of this\n");
}

int main() {
    // Testing a simple send and receive. Also testing the return of a coroutine
    Channel *ch1 = go(fun1);
    send(ch1, asString("Testing\n"));
    Value done = receive(ch1);
    printf("%s", done.asString);

    // Testing adding multiple sends to a channel (ideally testing for when these are sent first, could be that a received is asked first depending on implementation)
    Channel *ch2 = go(fun2);
    for (int i = 0; i < 10; i++) {
        send(ch2, asInt(i));
    }
    done = receive(ch2);
    printf("%s", done.asString);

    // Testing multiple receives from a channel (ideally testing for when these are received first, could be that a sent is done first depending on implementation)
    Channel *ch3 = go(fun3);
    for (int i = 0; i < 11; i++) {
        Value v = receive(ch3);
        printf("%d ", v.asInt);
    }
    done = receive(ch3);
    printf("%s", done.asString);

    // Testing infinite supply of return value
    Channel *ch4 = go(fun4);
    for (int i = 0; i < 2; i++) {
        send(ch4, asInt(i));
    }
    for (int i = 0; i < 50; i++) {
        done = receive(ch4);
        printf("%s", done.asString);
    }

    return 0;
}
