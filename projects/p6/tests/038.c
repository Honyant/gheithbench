#include <stdio.h>
#include <inttypes.h>

#include "src/go.h"

// test naming convention
Value _magic() {
    Channel* ch = channel();
    int counter = 0;
    for (int i = 0; i < 10; i++)
    {
        send(me(),asInt(i));
        counter += i;
    }
    send(me(), asInt(counter));
    receive(ch);
    return asLong(0);
}

Value send_through_channel() {
    Channel* ch = channel();
    send(me(), asInt(1234));
    receive(ch);
    return asLong(0);
}

// test naming convention
Value context_switch() {
    Channel *send_this_channel = go(send_through_channel);
    send(me(), asChannel(send_this_channel));
    return asLong(0);
}

int main() {
    Channel* _magic_ch = go(_magic);
    for(int i = 0; i < 11; i++) {
        int v = receive(_magic_ch).asInt;
        printf("received %d\n",v);
    }

    Channel *context_switch_ch = go(context_switch);
    Channel *received_channel = receive(context_switch_ch).asChannel;
    int received_num = receive(received_channel).asInt;
    printf("num sent through channel sent through channel: %d\n", received_num);
    receive(received_channel);

    printf("ruh roh\n");

    return 0;
}
