#include <stdio.h>

#include "src/go.h"

int againCount = 0;
int count2 = 0;

Value t1() { // this tests infinite sending after a function returns
    return asInt(5);
}

Value t2_2() {
    printf("this should print twice\n");
    return asInt(0);
}

Value t2_1() {
    Value v1 = t2_2();
    printf("%d\n", v1.asInt);
    return asInt(1);
}


Value t2() { //this tests having multiple functions as part of a
    Value v0 = t2_1();
    printf("%d\n", v0.asInt);
    if (count2 < 1) {
        count2++;
        again();
    }

    return asInt(2);
}

int main() {
    if (againCount == 2) {
        printf("again test passed!\n");
    }
    else {
        againCount++;
        again();
    }

    Channel* ch1 = go(t1);
    int a1 = receive(ch1).asInt;
    int a2 = receive(ch1).asInt;
    int a3 = receive(ch1).asInt;

    if (a1 != 5 || a2 != 5 || a3 != 5) {
        printf("infinite send not working\n");
    }
    else {
        printf("passed infinite send!\n");
    }

    Channel* ch2 = go(t2);
    a2 = receive(ch2).asInt;
    printf("multiple functions in routine test passed!\n");
}