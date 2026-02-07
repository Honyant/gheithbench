#include "src/go.h"
#include <stdio.h>
#include <stdlib.h>

/*
Welcome to my test case! This test case tests:
- Adding the same function via multiple coroutines
- Coroutine interaction with globals
- Calling again() in main (please do not throw a rock at my window)
- Program termination when no coroutines are available to run
- Whether there are squids in the water (spoiler: yes)
Have fun debugging and good luck!
*/

int x = 0;
int y = 1;
int z = 0;
int w = 727; // ysi
Channel* chan;

Value f1() {
    send(chan, asInt(w++));
    return asInt(494);
}

Value f2() {
    for (int i = 0 + z * 100; i < 99 + z * 100; i++) {
        send(me(), asInt(x++));
    }
    return asInt(618);
}

Value f3() {
    y /= 123;
    return asInt(123);
}

Value f4() {
    y *= 456;
    return asInt(456);
}

Value f5() {
    y /= 789;
    return asInt(789);
}

Value f6() {
    y *= 000;
    return asInt(000); 
}

int main() {
    //setvbuf(stdout, NULL, _IONBF, 0); // uncomment this line for debugging

    Channel* three = go(f3);
    Channel* four = go(f4);
    Channel* five = go(f5);
    Channel* six = go(f6);
    printf("%d\n", receive(three).asInt);
    printf("%d\n", receive(four).asInt);
    printf("%d\n", receive(five).asInt);
    printf("%d\n", receive(six).asInt);

    // No matter what order these coroutines go in, by the end, y should equal 0.
    printf("%d\n", y);

    // Send and receive should match up between main and f2
    Channel* a = go(f2);
    for (int i = 0 + z * 100; i < 99 + z * 100; i++) {
        printf("%d\n", receive(a).asInt);
    }

    // Make sure f2 returns work
    printf("%d\n", receive(a).asInt);

    if (z < 5) {
        printf("there are squids in the water\n");
        z++;
        again();
    }

    // call f2 so that we can block ourselves
    f2();

    // should never run
    while (true) {
        
    }
}