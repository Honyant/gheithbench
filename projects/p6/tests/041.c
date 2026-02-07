#include <stdint.h>
#include <stdio.h>
#include "src/go.h"

// BASIC TESTS
Value square() {
    long x = receive(me()).asLong;
    send(me(),asLong(x*x));
    return asLong(0);
}
int i = 0;
Value forLoop() {
    send(me(),asInt(i));
    if (i < 10) {
        i ++;
        again();
    }
    return asInt(0);
}
void testBasics() {
    // tests initialization
    Channel* c = channel();
    c = c;

    // tests send and receive basics
    Channel* squarer = go(square);
    send(squarer,asLong(100));
    printf("100^2 = %ld\n",receive(squarer).asLong);
    squarer = go(square);
    send(squarer,asLong(-20));
    printf("-20^2 = %ld\n",receive(squarer).asLong);

    // tests again basics
    Channel* for_looper = go(forLoop);
    while (1) {
        int x = receive(for_looper).asInt;
        if (x > 3) {
            break;
        }
        printf("%d, ",x);
    }
    printf("\n");
}


// COOL TEST - I did this for P2 too
Value tribonacci() {
    uint64_t n = receive(me()).asU64;
    // base
    if (n <= 2) {
        if (n == 0)
            send(me(),asU64(0));
        else
            send(me(),asU64(1));
        again();
    }
    // recursion
    Channel* trib = go(tribonacci);
    send(trib, asU64(n - 1));
    uint64_t n_1 = receive(trib).asU64;
    send(trib, asU64(n - 2));
    uint64_t n_2 = receive(trib).asU64;
    send(trib, asU64(n - 3));
    uint64_t n_3 = receive(trib).asU64;

    send(me(), asU64(n_1+n_2+n_3));
    again();

    // ERROR
    return asChar('L');
}
void testCoolFunction() {
    for (int i = 0; i < 25; i++) {
        Channel* trib = go(tribonacci); // create a new channel for each tribonnaci call
        send(trib, asU64(i));
        printf("%ld\n", receive(trib).asU64);
    }
}

int main() {
    // tests basics + cool function
    testBasics(); printf("BASIC TESTS PASSED!\n");
    testCoolFunction(); printf("COOL FUNCTION PASSED!\n");
    // sanity check
    receive(channel());
    printf("should never see this\n");
    return 0;
}