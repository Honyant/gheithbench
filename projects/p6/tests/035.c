//tests stack alignment
//tests freeing coroutine stacks and unused channels

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "src/go.h"

void * space;
uint64_t a;
Channel * c;

Value stack_aligment_test() {
    float val = 222.42;
    printf("%.2f\n", val);
    return asLong(0);
}

Value free_routine_test() {
    return asLong(0);
}

Value free_channel_test() {
    receive(c);
    return asLong(0);
}

int main() {
    Channel * ch = go(stack_aligment_test);
    receive(ch);
    printf("alignment test passed\n");
    fflush(stdout);

    //filling up memory
    for(uint64_t i = 36; i > 15; i--) {
        size_t len= 1L << i;
        while(true) {
            space = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED | 0x20, -1, 0);
            if (space == MAP_FAILED) {     
                i--;
                break;
            }
        }
    }
      
    //lots of coroutines (test free as you go)
    for(uint64_t i = 0; i < 1 << 8; i++){
        a = receive(go(free_routine_test)).asU64;
    }

    printf("routine free test passed\n");
    fflush(stdout);


    //lots of channels (test free unused channels)
    for(uint64_t i = 0; i < 1 << 10; i++){
        c = channel();
        go(free_channel_test);
        send(c,asInt(0));
    }

    printf("routine channel test passed\n");
    printf("done\n");
    fflush(stdout);

    return 0;
}


