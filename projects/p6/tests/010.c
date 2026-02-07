#include <stdio.h>
#include <inttypes.h>

#include "src/go.h"

Channel * mainCh = NULL;
Channel * justSendCh = NULL;
Channel * infiniteCh = NULL;
Channel * o1Ch = NULL;
Channel * o2Ch = NULL;


Value basic() {
    send(me(), asLong(1));
    receive(me());
    printf("failure1\n");
    return asLong(-1);
}

Value fastReturn(){
    return asLong(2);
}

Value justSend();

Value infinite(){
    receive(mainCh);
    send(justSendCh, asLong(-1));
    return asLong(3);
}

Value justSend(){
    receive(mainCh);
    send(infiniteCh, asLong(1234));
    receive(justSendCh);
    return asLong(-1);
}

Value o2();

Value o1(){
    receive(mainCh);
    send(o2Ch, asLong(-1));
    send(me(), asLong(2));
    return asLong(4);
}

Value o2(){
    receive(mainCh);
    send(o1Ch, asLong(1));
    receive(me());
    return asLong(-1);
}

int main() {
    //1: testing basic send and recieve
    mainCh = me();
    Channel* ch1 = go(basic);
    printf("Passed: %ld\n", receive(ch1).asLong);

    //2: testing recieve from infinite channel
    Channel* ch2 = go(fastReturn);
    printf("Passed: %ld\n", receive(ch2).asLong);

    //3: testing that if a routine is waiting to send when a function returns we can still recieve that value
    infiniteCh = go(infinite);
    justSendCh = go(justSend);

    send(me(), asLong(0));
    send(me(), asLong(0));
    printf("Value: %ld\n", receive(infiniteCh).asLong);
    printf("Value: %ld\n", receive(infiniteCh).asLong);
    printf("Passed: %ld\n", receive(infiniteCh).asLong);

    //4: testing that order of sends is preserved even if a function has returned
    mainCh = me();
    o1Ch = go(o1);
    o2Ch = go(o2);
    send(me(), asLong(0));
    send(me(), asLong(0));
    printf("Value: %ld\n", receive(o1Ch).asLong);
    printf("Value: %ld\n", receive(o1Ch).asLong);
    printf("Passed: %ld\n", receive(o1Ch).asLong);


    return 0;
}
