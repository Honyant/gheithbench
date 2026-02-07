#include <stdio.h>

#include "src/go.h"

Channel *out;

Value returner() {
    Channel* newChannel = channel();
    return asChannel(newChannel); 
}

Value co1() {
    send(me(), asInt(1));
    receive(out);
    printf("Test 2 pass: sending/receiving through channel that's a global variable\n");
    return asInt(47); 
}

Value co2() {
    Channel* ch = receive(me()).asChannel; 
    send(ch, asInt(1));
    return asInt(0);
}

Value co3() {
    Channel* ch = receive(me()).asChannel; 
    receive(ch);
    printf("Test 4 passed: sending message through channel that was sent through a channel\n");
    return asInt(0);
}

int i = 0;
Value forLoop() {
    i++;
    if(i < 1000){
        again(); 
    }
    send(out, asInt(i));
    return asInt(i);
}

Value deadend(){
    receive(out);
    printf("This should not be printed\n");
    return asInt(0);
}

int main() {
    // basic tests
    out = channel();
    Channel* co1channel = go(co1);

    receive(co1channel); 
    printf("Test 1 pass: send/receive/me\n");
    send(out, asInt(1));

    for(int i = 0; i < 1000; i++){
        receive(co1channel);
    }
    printf("Test 3 pass: infinite supply when return\n");

    // return a channel and have coroutines communicate theough it
    Channel* retCh = go(returner);
    Channel* co2Ch = go(co2);
    Channel* co3Ch = go(co3);

    send(co2Ch, receive(retCh));
    send(co3Ch, receive(retCh));

    // again test 
    go(forLoop);
    int test = receive(out).asInt;
    if(test == 1000){
        printf("Test 5 pass: testing \"again\" function\n");
    }

    // test that dead end routines don't stop the program
    go(deadend); 
    printf("Test 6 pass: code still runs with dead coroutine\n");

    receive(out); 
    printf("This shouldn't be printed.\n");
    return 0;
}