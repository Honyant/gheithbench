#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>

#include "src/go.h"

int counter = 0;
Channel* chan = NULL;
bool pass = true;

Value test_recieve () {
    counter += receive(chan).asInt;
    printf("recieve queue counter: %d\n", counter);
    return asInt(0);
}

Value test_send () {
    send(chan, asInt(2));
    return asInt(987);
}

Value test_never_recieve() {
    receive(me());
    printf("__(0)< OH NO! You shouldn't be seeing me!!\n\\___)\n");
    pass = false;
    return asInt(0);
}

Value test_never_send() {
    send(me(), asInt(500));
    printf("__(0)< OH NO! You shouldn't be seeing me!!\n\\___)\n");
    pass = false;
    return asInt(0);
}

int main() {
    chan = channel();

    printf("__(.)< Hello! I'm Debugging Duck! I'll help you find bugs!\n\\___)\n");

    go(test_send);
    if(receive(chan).asInt != 2) {
        printf("__(.)< Oh No!! you aren't sending and recieving properly!\n\\___)\n");
        pass = false;
    }

    // tests
    go(test_recieve);
    go(test_recieve);
    go(test_recieve);
    go(test_recieve);
    go(test_recieve);

    go(test_send);
    go(test_send);
    go(test_send);
    go(test_send);
    Channel* test = go(test_send);

    for(int i = 0; i < 1000; i++) {
        Value v1 = receive(test);
        if (v1.asInt != 987) {
            printf("__(.)< Womp Womp you aren't sending infinite values\n\\___)\n");
            pass = false;
        }
    }

    go(test_never_recieve);
    go(test_never_send);

    if(pass) {
        printf("__(.)< Yayy I think you pass everything!!\n\\___)\n");
    }

    return 0;
}