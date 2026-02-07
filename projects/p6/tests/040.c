#include <stdio.h>

#include "src/go.h"

Value fibonacci() {
    long a = 1;
    long b = 1;

    while (1) {
        send(me(),asLong(a));
        a=a^b;
        b=a^b;
        a=a^b;
        a=a+b;
    }
    return asLong(-1);
}
Value testInfiniteSend(){
    send(me(), asLong(1));
    send(me(), asLong(2));
    return asLong(100);
}

Value block(){
    send(me(), asLong(2309472903));
    return asLong(0);
}


int main() {
    Channel* fib = go(fibonacci);
    for (long i = 0; i<50; i++) {
        long f = receive(fib).asLong;
        printf("%ld\n",f);
    }


    Channel* infinite_send=go(testInfiniteSend);

    for (long i = 0; i<100; i++) {
        long f = receive(infinite_send).asLong;
        printf("%ld\n",f);
    }
    
    Channel* blocked=go(block);
    send(blocked, asLong(32984));
    printf("should never print");


    return 0;
}
