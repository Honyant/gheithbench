#include <stdio.h>
#include "src/go.h"

//test me() in a function called by a coroutine
//(that is not itself a coroutine)

Channel * mainChannel;

// ~~~~~~~ test 1 stuff ~~~~~~~

Value f1() {
    while (true) {
        Value x = receive(me());
        printf("%i\n", x.asInt);
        if (x.asInt == 5) break;
    }
    return asString("f1 ran");
}

Value f2() {
    send(me(), asInt(10));
    return asString("f2 ran");
}

void test1() {

    Channel *f1ch = go(f1);
    Channel *f2ch = go(f2);

    for (int i = 0; i < 6; i++)
    {
        send(f1ch, asInt(i));
    }

    Value f1Ret = receive(f1ch);
    printf("%s\n", f1Ret.asString);

    Value f2Sent = receive(f2ch);
    printf("f2 sent: %i\n", f2Sent.asInt);
    f2Sent = receive(f2ch);
    printf("%s\n", f2Sent.asString);
}

// ~~~~~~~ test 2 stuff ~~~~~~~

int f3Counter = 0;

Value sendOnMain() {
    for (int i = 10; i > 0; i--) {
        send(mainChannel, asLong(i));
    }
    return asShort(3);
}

void test2() {
    //Channel * mainSenders = go(sendOnMain);
    go(sendOnMain);
    for (int i = 0; i < 10; i++) {
        long x = receive(me()).asLong;
        if (x + i != 10) {
            printf("sending/receiving in wrong order\n");
        }
    }
}

// ~~~~~~~ test 3 stuff ~~~~~~~
void square(int x) {
    send(me(), asInt(x*x));
}

Value test3_1() {
    square(5);
    return asChar('x');
}

Value test3_2() {
    square(8);
    return asChar('x');
}

//there's no reason this shouldn't work
void test3() {
    Channel *ch3_1 = go(test3_1);
    Value five = receive(ch3_1);
    if (five.asInt != 25) {
        printf("test 3-1 went wrong :(\n");
        printf("this one should be no different than usual send/receive\n");
    }

    Channel *ch3_2 = go(test3_2);
    Value eight = receive(ch3_2);
    if (eight.asInt != 64) {
        printf("test 3-2 went wrong :(\n");
        printf("check channels of coroutines?\n");
    }
}

int main() {
    printf("test 1: basic send/receive\n");
    test1();
    printf("test 1 passed!\n\n");

    printf("test 2: main has a channel\n");
    mainChannel = me();
    if (mainChannel == NULL) {
        printf("main needs a channel =C\n");
    }
    test2();
    printf("test 2 passed!\n\n");

    printf("test 3: me() in functions called by coroutines\n");
    test3();
    printf("test 3 passed!\n\n");

    //test exit if everything blocked
    printf("test 4: exit on main() being blocked\n");
    printf("bye bye (:\n");
    receive(me());
    printf("hello again! you shouldn't see this line...\n");

    return 0;
}