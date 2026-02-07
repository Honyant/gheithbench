#include <stdio.h>
#include "src/go.h"

// names of functions are possible naming collisions
int c = 0;
Value magic() {
    if (c < 7) {
        c++;
        printf("c is %d\n", c);
        again();
    }
    return asLong(0); 
}

uint64_t val1 = 0;
uint64_t val2 = 0;
Value initialize() {
    val1 = 2222;
    long r2 = receive(me()).asLong;
    r2 += 1;
    return asLong(0);
}

Value yield() {
    val2 = 1111;
    Channel* b = go(initialize);
    long r = receive(me()).asLong;
    send(b, asLong(r));
    return asLong(0);
}

Value context_switcher() {
    uint64_t num = receive(me()).asU64;
    uint64_t the_best_number = num + 63;
    return asU64(the_best_number);
}

int main() {
    // this tests sending and receiving within functions
    Channel* a = go(yield);
    send(a, asLong(10));
    

    // this tests that, when a function returns, it sends infinitely :)
    int incorrectInfCount = 0;
    Channel* c = go(context_switcher);
    send(c, asU64(10));
    for (int i = 0; i < 100; i++) {
        uint64_t fromContextSwitcher = receive(c).asU64;
        if (fromContextSwitcher != 73) {
            incorrectInfCount++;
        }
    }
    if (incorrectInfCount > 0) {
        printf("Channel c sends %d / 100 incorrect values during the infinite sending test \n", incorrectInfCount);
    } else {
        printf("Infinitely sending test passed!\n");
    }

    // this tests "again"
    Channel* d = go(magic);
    receive(d);

    // continuation of sending and receiving test
    printf("Value of val1 is equal to %ld\n", val1);
    printf("Value of val2 is equal to %ld\n", val2);

    // this tests the program after blockage
    printf("Last line!\n");
    Channel* blocky = channel();
    receive(blocky);
    printf("should not see this\n");
    
    return 0;
}