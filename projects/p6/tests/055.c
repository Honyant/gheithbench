#include "src/go.h"
#include <stdio.h>
#include <stdbool.h>

int fib = 10;

bool factorial_done = false;
bool dead_channel_done = false;
bool stress_test_done = false;

Channel *factorialChannel;
Channel *stressTestChannel;

Value factorial10(){
    if (fib <= 1) {
        send(factorialChannel, asLong(1));
        return asLong(1);
    }

    Value v = asLong(fib);
    v = asLong(v.asLong * factorial10(--fib).asLong);
    send(factorialChannel, v);
    return v;
}

Value dead_channel(){
    return asLong(4);
}

Value stress_test_main_sender(){
    for (int i = 0; i < 10; i ++) {
        send(me(), asString("Hello World!"));
    }
    return asLong(0);
}

Value stress_test_send_single() {
    send(stressTestChannel, asString("Hola\n"));
    return asLong(0);
}

Value stress_test_receiver() {
    printf("%s", receive(stressTestChannel).asString);
    return asLong(0);
}


int main(int argc, char** argv) {
    // Testing argc
    printf("argc is %d\n", argc);

    if (!factorial_done) {
        factorialChannel = channel();
        // Testing Recursion
        go(&factorial10);
        while (receive(factorialChannel).asLong  != 10 * 9 * 8 * 7 * 6 * 5 * 4 * 3 * 2 * 1) {
            printf("Factorial Running\n");
        }
        factorial_done = true;
    } else if (!dead_channel_done) {
        Channel* ch = go(&dead_channel);
        for (int i = 0; i < 20; i ++) {
            printf("Receiving From Dead Channel: %ld\n", receive(ch).asLong);
        }
        dead_channel_done = true;
    } else if (!stress_test_done) {
        stressTestChannel = channel();

        for (int i = 0; i < 100; i ++) {
            go(&stress_test_send_single);
        }

        for (int i = 0; i < 1000; i ++) {
            go(&stress_test_receiver);
        }

        Channel *ch = go(&stress_test_main_sender);
        for (int i = 0; i < 10; i ++) {
            receive(ch);
        }
        stress_test_done = true;
    }
    

    if (factorial_done && dead_channel_done && stress_test_done) {
        send(channel(), asString("Hanging main"));
    }

    again();
}