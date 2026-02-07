#include <stdio.h>
#include "src/go.h"
#include <stdbool.h>

Value f1() {
    int c = 0;
    while(1) {
        send(me(), asInt(c));
        c++;
    }
    return asLong(0);
}

Value f2() {
    send(me(), asInt(5));
    return asInt(8);
}

int a = 5;
Value f3() {
    if(a < 10) {
        printf("%d ", a);
        a++;
        again();
    }
    printf("\n");
    a = 5;
    send(me(), asInt(1));
    return asInt(123);
}

bool repeated = false;
int main(int argc, char** argv) {
    printf("hi\n");
    if(!repeated) {
        repeated = true;
        again();
    }

    Channel* ch = go(f1);
    for(int i = 0; i < 5; i++) {
        int x = receive(ch).asInt;
        printf("%d\n", x);
    }

    Channel* ch2 = go(f2);
    int a = receive(ch2).asInt;
    int b = receive(ch2).asInt;
    printf("%d %d\n", a, b);
    for(int i = 0; i < 5; i++)
        printf("%d ", receive(ch2).asInt);
    printf("\n");

    Channel* ch3 = go(f3);
    receive(ch3);

    if(argc != 1) printf("This shouldn't be printed.\n");
    
    return 0;
}