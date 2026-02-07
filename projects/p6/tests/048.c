#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "src/go.h"


int toggle = 0;

Value sendVal() {
    send(me(), asU64((uint64_t)toggle));
    again();
    return asU64(0);
}

int main() {
    // calling again inside main
    Channel* child = go(sendVal);
    uint64_t recv = receive(child).asU64;
    printf("%ld\n", recv); // 0, 1
    toggle ^= 1;
    if (toggle) again();

    return 0;
}
