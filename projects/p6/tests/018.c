#include <stdio.h>
#include <inttypes.h>

#include "src/go.h"

uint64_t timesMainHasRun = 0;

Value increaseCharByOne() {
    char x = receive(me()).asChar;
    send(me(), asChar(x - 1));
    again();
    
    return asLong(0);
}

Value infiniteSupply() {
    return asLong(2);
}

int main() {
    timesMainHasRun++;

    if (timesMainHasRun == 1) {
        printf("again() & coroutines test!\n");
    }

    Channel* printer = go(increaseCharByOne);

    // this string is 'starting test' but each character is shifted down one in ascii
    char* string = "tubsujoh!uftu";
    for (int i = 0; i < 13; i++) {
        // recieve should block this channel so it will only
        send(printer, asChar(string[i]));
        char x = receive(printer).asChar;
        printf("%c", x);
    }
    printf(": %ld\n", timesMainHasRun);

    // lets run the above 5 times
    if (timesMainHasRun != 5) {
        again();
    }

    printf("Test that when coroutine returns, it supplies infinite sends to the channel:\n");

    // infinite supply of couroutines
    Channel* infiniteChannel = go(infiniteSupply);
    uint64_t cumulative = 0;
    for (int i = 0; i < 100000; i++) {
        cumulative += receive(infiniteChannel).asLong;
    }
    printf("%ld\n", cumulative);
    

    return 0;
}
