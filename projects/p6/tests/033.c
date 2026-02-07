#include <stdio.h>
#include <inttypes.h>

#include "src/go.h"

static int again_counter = 0;
static int main_counter = 0;

Value f1() {
    send(me(), asLong(35));
    return asLong(0);
}

Value f2() {
    Value v = receive(me());
    return asLong(v.asLong - 20);
}

Value f_again_infinite() {
    if (again_counter <  10) {
        printf("Again counter: %d\n", again_counter);
        again_counter++;
        again();
    }
    return asLong(60000);
}

Value blocker() {
    // this method never recieves main's sent value
    return asLong(0);
}

int main() {
    // testing again on main
    if (main_counter < 4) {
        printf("Main counter: %d\n", main_counter);
        main_counter++;
        again();
    }

    // testing basic send and receive functionalities
    Channel* ch1 = go(f1);
    Channel* ch2 = go(f2);
    Value v1 = receive(ch1);
    send(ch2, asLong(45));
    printf("Main successfully sent value 45 to f2 and became unblocked.\n");
    if (v1.asLong == 35) {
        printf("Main successfully received value 35 from f1 and became unblocked.\n");
    }

    // testing general again functionality
    Channel* ag_inf = go(f_again_infinite);
    Value ag_val = receive(ag_inf);
    printf("Value received after again: %ld\n", ag_val.asLong); // should be return value (60000)

    // testing infinite return
    for (int i = 0; i < 3; i++) {
        Value inf_val = receive(ag_inf);
        printf("Loop %d: receiving value should be the same as above: %ld\n", i, inf_val.asLong); // should be return value (60000)
    }

    // blocked routine
    Channel* block_ch = go(blocker);
    send(block_ch, asLong(10000000));
    printf("This line should never be reached since main is blocked.\n");
    
    return 0;
}