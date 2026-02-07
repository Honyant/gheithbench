#include <stdio.h>

#include "src/go.h"

uint64_t x = 0;
uint64_t iterations = 0;

Value f() {
    uint64_t y = receive(me()).asU64;
    x += y;
    send(me(), asU64(5));
    return asU64(5);
}

Value g() {
    return asU64(10);
}

Value a() {
    Channel* b_ch = receive(me()).asChannel;
    send(b_ch, asInt(5));
    int x = receive(b_ch).asInt;
    send(me(), asInt(x));
    return asInt(5);
}

Value b() {
    int x = receive(me()).asInt;
    x += 5;
    send(me(), asInt(x));
    return asInt(5);
}

int main() {
    // test alternating execution between functions
    Channel* f_ch = go(f);
    x++;
    send(f_ch, asU64(5));
    x += receive(f_ch).asU64;
    if (x == 11 * (iterations + 1)) {
        printf("%s\n", "Passed test 1");
    }

    // test recieving from a finished routine
    Channel* g_ch = go(g);
    uint64_t count = 0;
    for (uint64_t i = 0; i < 10; i++) {
        count += receive(g_ch).asU64;
    }
    if (count == 100) {
        printf("%s\n", "Passed test 2");
    }

    // pass values between multiple coroutines
    Channel* a_ch = go(a);
    Channel* b_ch = go(b);

    send(a_ch, asChannel(b_ch));
    int result = receive(a_ch).asInt;

    if (result == 10) {
        printf("%s\n", "Passed test 3");
    }

    // Do it all again
    if (iterations < 3) {
        iterations++;
        again();
    }

    return 0;
}
