#include "src/go.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int stay_count = 0;
int sender_count = 0;
bool first = true;
int total = 0;
int _argc_;
char** _argv_;
Channel* multiple;

Value stay() {
    // stack pointer should revert to top of frame upon again()
    while (stay_count < 10000) {
        int stuff[100];
        stuff[0] = 666;
        stuff[99] = 108;
        if (stay_count == 0) {
            printf("product: %d\n", stuff[0] * stuff[99]);
        }
        if (stay_count == 9876) {
            printf("sum: %d\n", stuff[0] + stuff[99]);
        }
        stay_count++;
        again();
    }
    return asLong(1);
}

Value sender() {
    sender_count++;
    send(multiple, asLong(sender_count));
    return asLong(sender_count);
}

int main(int argc, char** argv, char** envp) {
    if (first) {
        printf("Hello!\n");
        _argc_ = argc;
        printf("argc (first iteration): %d\n", argc);
        _argv_ = malloc(argc * sizeof(char*));
        for (int i = 0; i < argc; i++) {
            _argv_[i] = malloc(sizeof(char) * (strlen(argv[i]) + 1));
            strcpy(_argv_[i], *argv);
        }
    } else {
        printf("argc (second iteration): %d\n", argc);
        if (argc != _argc_) {
            printf("argc test failed (doesn't match first call to main).\n");
        }
        for (int i = 0; i < _argc_; i++) {
            if (strcmp(_argv_[i], argv[i])) {
                printf("argv test failed (doesn't match first call to main).\n");
            }
        }
    }

    if (first) {
        Channel* stayCh = go(stay);
        receive(stayCh);
        // test successive senders on same channel
        multiple = channel();
        for (int i = 0; i < 10; i++) {
            go(sender);
            printf("%ld\n", receive(multiple).asLong);
        }
    }

    // test main stack size
    int large_array[12345];
    large_array[0] = 666;
    large_array[12344] = 108;
    printf("Remainder: %d\n", large_array[0] % large_array[12344]);

    // main should operate on the same stack after again()
    if (first) {
        first = false;
        again();
    }

    // Yay!
    printf("Congratulations!\n");
}