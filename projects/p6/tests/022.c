#include <stdio.h>
#include <inttypes.h>

#include "src/go.h"
#include <string.h>

Value simpleRequest() {
    Value v = asInt(0);
    send(me(), v);
    return v;
}

Value infiniteReturner() {
    Value v = asString("infinity");
    send(me(), v);
    return v;
}

Value myFunctionSwitch() {
    printf("Correctly forced context switch away from main.\n");
    printf("This is the last thing printed!\n");
    receive(me());
    return asInt(5);
}

int againCount = 1;
Value againTest() {
    //testing again call within a coroutine
    //this should print 10 times
    if (againCount < 11) {
        printf("again call ");
        printf("%d\n", againCount);
        againCount++;
        again();
    }
    receive(me());
    return asInt(100);
}

int main() {

    //let's make a simple send and receive across a channel
    Channel* simple = go(simpleRequest);
    printf("%d\n", receive(simple).asInt);

    //testing that a channel infinitely sends the return value
    Channel* infinite = go(infiniteReturner);
    printf("%s\n", receive(infinite).asString);
    
    for (int i = 0; i < 1000; i++) {
        if (strcmp(receive(infinite).asString,"infinity")) {
            printf("Channel that returned did not infinite-send its value\n");
        }
    }

    //let's test again on a coroutine
    Channel* callAgain = go(againTest);
    send(callAgain, asInt(4));

    Channel* myFunction = go(myFunctionSwitch);

    //let's make sure me() works for main
    Channel* main_channel = me();

    //main is blocked, so we should force a context switch to the only available routine (myFunctionSwitch)
    //or, depending on execution order, we ran the other routine first and both will now be blocked
    send(main_channel, asInt(5));

    //testing that we end the program when all active channels are blocked
    //all channels are blocked after the receive() call on myFunction channel, so we should end the program here
    printf("This should NOT be printed!");

    //This line never runs but is needed to avoid "unused variable" compiler error
    receive(myFunction);

    return 0;
}
