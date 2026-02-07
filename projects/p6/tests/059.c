#include <stdio.h>
#include "src/go.h"

Value f2() {
    int temp = 1;
    return asInt(temp); 
}

void printingNumbers(int num) {
    printf("%d\n", num);
}

Value f1() {
    printf("wow made it to f1\n");
    // Tests again on regular function
    static int itr = 0;
    if(itr < 10) {
        printingNumbers(itr); 
        itr++;
        again();
    }
    else {
        printf("itr is greater than 9\n");
    }
    send(me(), asInt(1));
    return asInt(100);
}

int main() {
    // Tests go
    Channel* ch1 = go(f1);
    // Blocks main until a receive from ch1
    Value temp = receive(ch1);
    printf("Completed ch1: %d\n", temp.asInt);
    // Tests again on main
    static int numAgainMain = 0;
    while(numAgainMain < 2) {
        printf("Doing again on main\n");
        numAgainMain += 1;
        again();
    }
    
    Channel* ch2 = go(f2);
    receive(ch2);

    // Makes sure can receive infinite value
    if (receive(ch2).asInt) {
        printf("wow took return value as a send!\n");
        return 0;
    }
    else {
        printf("didn't pass return value so correctly\n");
        return 1;
    }
}
