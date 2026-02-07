#include <stdio.h>
#include <inttypes.h>
#include "src/go.h"

// this testcase tests whether your coroutine channel returns correct values after finishing

Value f() {
    for(int i = 0; i < 10; i ++) {
        send(me(), asInt(i));
    }
    return asInt(35);
}

int main() {
    Channel* child = go(f);

    for(int j = 0; j < 20; j ++) {
        int x = receive(child).asInt;
        printf("%d\n", x);
    }

    printf("complete\n");
    return 0;
}
