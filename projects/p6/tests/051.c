#include <stdio.h>
#include <inttypes.h>

#include "src/go.h"
#include <stdlib.h>


int againTester = 0;

// send tester
Value sendTester() {
    return asInt(1);
}

// receive tester
Value receiveTester() {
    int i = receive(me()).asInt;
    printf("%d\n", i);
    return asInt(i*2);
}

// again tester should print out count 10 times
Value againTEST() {
    if (againTester < 1) {
        printf("five\n");
        againTester++;
        again();
    }
    return asInt(6);
}

// test coroutines
Value coroutineTest() {
    int rec = receive(me()).asInt;
    send(me(), asInt(rec*2));
    return asInt(rec*3);
}
Value coroutineA();
Value coroutineB();
// test dual coroutines
int recursion = 10;


// calls each other's couroutine until the end condition is reached and prints out a recursion counter
Value coroutineA() {
    if (recursion == 123) {
        return asInt(123);
    }
    if (recursion == 10) {
        int r = receive(me()).asInt;
        printf("%d\n", r);
    }
    Channel* ba = go(coroutineB);
    printf("a\n");
    recursion--;
    send(ba, asInt(recursion));
    return asInt(3);
}

Value coroutineB() {
    Channel* ba = go(coroutineA);
    printf("b\n");
    recursion-=2;
    int rec = receive(me()).asInt;
    printf("%d\n", rec);
    if (rec<= 0) {
        rec = receive(ba).asInt;
        recursion = 123;
    }
    return asInt(4); 
}


int main() {
    // send test
    Channel* b = go(sendTester);
    int y = receive(b).asInt;
    printf("%d\n", y);

    // recieve test
    Channel* c = go(receiveTester);
    send(c, asInt(2));
    int z = receive(c).asInt;
    printf("%d\n", z);

    // again test
    Channel* a = go(againTEST);
    int x = receive(a).asInt;
    printf("%d\n",x);

    // infinite send test
    for (int i = 0; i < 100; i++) {
        int ii = receive(b).asInt;
        if (i % 32 == 0) printf("%d\n", ii*7);
    }

    // coroutine test
    Channel* d = go(coroutineTest);
    send(d, asInt(32));
    int rec = receive(d).asInt - 8*7;
    if (rec == 32) {
        printf("big error\n");
    }
    printf("%d\n", rec);
    Channel* e = go(coroutineA);
    send(e, asInt(4));
    send(e, asInt(5));

    return 0;
}
