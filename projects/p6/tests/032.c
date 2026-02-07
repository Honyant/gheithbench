#include <stdio.h>
#include "src/go.h"

//tests send functionality
Value testSend() {
    int i;
    for (i = 0; i < 10; i+=2) {
        send(me(), asInt(i));
    }
    return asInt(i);
}

//tests receive functionality
Value testReceive() {
    Value ret;
    for (int i = 0; i < 6; i++) {     
        ret = receive(me());
    }
    return ret;
}

//tests again functionality
Value testAgain() {
    for (int i = 0; i < 3; i++) {
        send(me(), asInt(i));
        again();
    }
    return asInt(20);
}

int main() {
    Channel* ch = go(testSend);
    for (int i = 0; i < 6; i++) {
        //should print "0 2 4 6 8 10"
        printf("%d\n", receive(ch).asInt);
    }

    ch = go(testReceive);
    for (int i = 10; i >= 5; i--) {
        send(ch, asInt(i));
    }

    //tests that testReceive's last received value is correct
    //also tests that return value is sent indefinitely
    for (int i = 0; i < 3; i++) {
        //should print "5 5 5"
        printf("%d\n", receive(ch).asInt);
    }

    //tests that again immediately restarts routine
    Channel* againCh = go(testAgain);
    for (int i = 0; i < 5; i++) {
        //should print "0 0 0 0 0"
        printf("%d\n", receive(againCh).asInt);
    }
    printf("yay we're done!!");
    send(againCh, asInt(-2));
    printf("should not be here :(");
    
    return 0;
}