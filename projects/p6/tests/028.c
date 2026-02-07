#include <inttypes.h>
#include <stdio.h>

#include "src/go.h"

int count = 0;

Value againTesting() {
    // testing rsp reset every time again() is called
    // stack should not be reused
    uint64_t arr[1000];
    arr[0] = 3245902;
    arr[999] = 902332;
    arr[998] = arr[0] + arr[999];

    if (count < 5) {
        count++;
        again();
    }

    send(me(), asLong(0xcafe));
    return asInt(1001);
}

Value blockTesting() {
    // testing send/receiving when blocked
    for (int i = 3; i >= 0; i--) {
        send(me(), asInt(i));
    }
    return asLong(1234);
}

int main() {
    Channel* ch = go(againTesting);

    long x = receive(ch).asLong;
    printf("Wow, I'd like some %lx\n", x);

    int y = 0xdee;

    Channel* ch2;
    if (count == 5) {
        ch2 = go(blockTesting);

        // sends 4 times but only receives 2 times
        for (int i = 0; i < 2; i++) {
            y = receive(ch2).asInt;
        }

        if (y != 2) {
            printf("Error: buffered send/receive incorrect\n");
            return 0;
        }

        int remaining = 2;
        // test infinite sending when function returns
        for (int i = 0; i < 10; i++) {
            if (receive(ch).asInt != 1001) {
                printf("Infinite receive for againTesting function error: index(%d)\n", i);
                return 0;
            }

            // due to short circuiting the receive shouldn't run until the remaining 2 sends have been paired
            if (remaining == 0 && receive(ch2).asLong != 1234) {
                printf("Infinite receive for blockTesting function error: index(%d)\n", i);
                return 0;
            } else {
                printf("%d\n", receive(ch2).asInt);
                remaining--;
            }
        }
    }

    // this will test calling again in main 2 times
    if (count < 7) {
        count++;
        again();
    }

    printf("This is the last line\n");
    send(channel(), asInt(50));

    printf("Uh Oh Stinky!! :(\n");

    return 0;
}
