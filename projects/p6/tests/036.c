#include <stdio.h>
#include <stdbool.h>
#include "src/go.h"

// Test case is less than 2000 characters without comments

uint64_t ctrForA1 = 0;

// recursive coroutine that also tests again()
Value a1() {
    long long x = receive(me()).asLongLong;
    if (x <= 1) {
        send(me(), asLongLong(0));
        again();
    }
    Channel* recursion = go(a1);

    // recursively compute number of times it takes for x to become 1 using collatz conjecture rules
    if (x % 2 == 0) {
        ctrForA1++;
        send(recursion, asLongLong(x / 2));
        receive(recursion);
    } else {
        ctrForA1++;
        send(recursion, asLongLong(3 * x + 1));
        receive(recursion);
    }
    send(me(), asLongLong(ctrForA1));
    again();

    // should never return this
    return asLongLong(-5);
}

// used for the infinite return test
Value simple() {
    receive(me());
    return asLongLong(-1);
}

// tests a series of co-routine sends/receives on the recursive a1()
Value a2() {
    // ensuring multiple channels can initialize co-routines for the same function
    Channel* b1 = go(a1);
    Channel* b2 = go(a1);
    Channel* b3 = go(simple);

    // summing the total values that a1() returns from i = 20..40; while using the multiple channels
    long long sum = 0;
    for (int i = 20; i <= 40; i++) {
        ctrForA1 = 0;
        if (i % 2 == 0) {
            send(b1, asLongLong(i));
            sum += receive(b1).asLongLong;
        } else {
            send(b2, asLongLong(i));
            sum += receive(b2).asLongLong;
        }
    }

    // ensuring that infinite return works
    send(b3, asLongLong(5));
    for (int i = 0; i < 100; i++) {
        if (receive(b3).asLongLong != -1) {
            printf("Infinite return is not working\n");
        }
    }

    // indicating this function has finished
    send(me(), asLongLong(sum));

    return asLongLong(0);
}

int main() {
    Channel* ch1 = go(a1);

    printf("Test 1:\n"); // testing recursive functions using coroutines to switch execution
    for (long i = 20; i <= 40; i++) {
        ctrForA1 = 0;
        send(ch1, asLongLong(i));
        long long collatzRes = receive(ch1).asLongLong;
        printf("%ld:  result: %lld\n", i, collatzRes);
    }

    // test 2 (testing sends/receives on multiple channels and infinite returns from a channel)
    Channel* ch2 = go(a2);
    printf("Test 2:\n");
    long long collatzSums = receive(ch2).asLongLong;
    printf("Result for test 2: %lld\n", collatzSums);

    printf("Tests are over!\n");

    return 0;
}