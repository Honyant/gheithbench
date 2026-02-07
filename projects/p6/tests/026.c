#include <stdio.h>
#include <inttypes.h>

#include "src/go.h"

Value f1() {
    return asLong(100);
}

Value f2() {
    Channel* c = channel();
    send(me(), asChannel(c));
    printf("%s", receive(c).asString);
    return asInt(0);
}

Value f3() {
    Channel* c = receive(me()).asChannel;
    send(c, asString("f3 just sent a message to f2!\n"));
    return asInt(0);
}

Value f4() {
    printf("test\n");
    if (receive(me()).asInt == 1) {
        again();
    }
    printf("f4 done!\n");
    return asInt(0);
}

int main() {
    uint64_t arr[9000]; // creating an array that is larger than stack size for a coroutine
    for (int i = 0; i < 9000; i++) {
        arr[i] = 2 * i; 
    } 
    printf("%ld\n", arr[0]);
    printf("%ld\n", arr[8999]);
    

    // testing copied values after returning
    Channel* ch = go(f1);
    arr[0] = receive(ch).asLong;
    for (int i = 1; i < 9000; i++) {
        arr[i] = receive(ch).asLong;
        if (arr[i] != arr[i - 1]) {
            printf("FAIL! After a coroutine returns, the return value should be repeated!");
        }
    }
    printf("Coroutine returns work as expected!\n");

    // sending channels between coroutines
    Channel* ch2 = go(f2);
    Channel* ch3 = go(f3);
    Channel* c = receive(ch2).asChannel;
    send(ch3, asChannel(c));
    receive(ch2);
    printf("This should print after the message was sent!\n");

    // testing again
    Channel* ch4 = go(f4);
    printf("testing again() ...\n");
    send(ch4, asInt(1)); // prints "test\ntest\n"
    send(ch4, asInt(1)); // prints "test\ntest\n"
    send(ch4, asInt(0)); // prints "f4 done\n"

    receive(channel());
    printf("if you see this message, you messed up. (shouldve terminated)\n");

    return 0;
}
