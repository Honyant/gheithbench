#include <stdio.h>
#include "src/go.h"
int numAgains = 0;
Value t4() {
    if (numAgains < 11) {
        send(me(), asInt(10));
        numAgains++;
        again();
    }
    return asString(".");
}
Value t1() {
    int i = receive(me()).asInt;
    printf("%d\n", i);
    return asInt(0);
}
Value t2() {
    send(me(), asInt(2));
    send(me(), asInt(3));
    send(me(), asInt(4));
    Value x = receive(me());
    return x;
}
int main(int argc, char* argv[]) {
    printf("This line and the next 5 lines should be printed twice\n");
    //test 1: test basic send + recieve
    Channel *c1 = go(t1);   
    send(c1, asInt(1));

    //test 2: test send and recieve chain with return value
    Channel *c2 = go(t2);
    int x = receive(c2).asInt;
    int y = receive(c2).asInt;
    int z = receive(c2).asInt;
    send(c2, asInt(5));
    int t2 = receive(c2).asInt;
    printf("x: %d\ny: %d\nz: %d\nt2: %d\n", x, y, z, t2);

    //test 3: test argc + argv preservation with again on main
    argc = 2;
    argv[0] = "hello";
    if (numAgains == 0) {
        numAgains++;
        again();
    }
    printf("argc: %d\n", argc);
    printf("argv[0]: %s\n", argv[0]);

    //test 4: test again on different function
    Channel *c4 = go(t4);
    
    for (int i = 0; i < 10; i++) {
        int x = receive(c4).asInt;
        printf("%d ", x - i);
    }

    //test 5: testing infinite send
    for (int i = 0; i < 3; i++) {
        char* x = receive(c4).asString;
        printf("%s", x);
    }
    printf("\nLiftoff!\n");

    return 0;
}