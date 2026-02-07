#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "src/go.h"

//tests making a lot of channels and running again on them
//also tests embedded go functions to ensure proper blocking with no race conditions

int counter = 0;

Value empty_channel_test() {
    //make new random empty channel
    //current should be main here so should 
    Channel* empty_channel = channel();

    printf("new channel yayyyy\n");
    send(me(), asU64(7));
    receive(empty_channel);
    printf("Should not print as it is an empty channel and no sending pairs\n");

    return asU64(1);

}

Value stress_channels() {
    for (int i = 0; i < 11000; i++) {
        send(me(), asU64(i));
    }

    return asU64(7777777);
}

Value channels() {
    Channel *test_channel = go(stress_channels);

    for (int i = 0; i < 11000; i++) {
        uint64_t rec = receive(test_channel).asU64;
        if (rec != i) {
            printf("%s", "Check recieve function as the value is not correct :(\n");
        }
    }

    return asU64(2);
}

Value inner_func() {
    send(me(), asU64(1234));
    return asU64(3);

}

Value embedded_func() {
    Channel *inner_channel = go(inner_func);

    //blocking specifically so the print statement is executed
    uint64_t recieved = receive(inner_channel).asU64;

    printf("%lu\n", recieved);

    return asU64(4);
}

Value embedded_test() {
    Channel *channel_1 = go(embedded_func);

    //blocking with recieve specifically so that print occurs
    receive(channel_1);
    printf("Yayyy it properly yielded and called the embedded functions!\n");

    return asU64(5);
}

int main() {

    //testing the initialization of many sends and recieves and if they are recieving and returning the right value
    channels();

    //should do this stress channel test again and again and free everything
    if (counter < 11000) {
        counter++;
        again();
    }

    //empty random channel test with nothing sending behavior check
    Channel *empty_test = go(empty_channel_test);
    receive(empty_test);

    //call go within a function that calls go for the embedded coroutines and checks blocking
    embedded_test();

    printf("%s", "Yayyyy you passed <3 <3 <3!\n");

    return 0;
}