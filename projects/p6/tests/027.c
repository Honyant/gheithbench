#include <stdio.h>

#include "src/go.h"

Channel* cGlobal1;

bool isFirst = true;

Value blockTest1();

Value blockTest2();

Value sanity() {
    return asInt(1);
}

Value sideEffect();

int mutated = 0;

int accumulator = 0;

int main() {

    Channel* sanityChannel = go(sanity);

    Value sanityVal = receive(sanityChannel);

    Value repeatTest = receive(sanityChannel);

    if (sanityVal.asInt == 1 && repeatTest.asInt == 1) {
        printf("passed repeated receive test!\n");
    }

    //test blocking
    cGlobal1 = channel();

    Channel* c1 = go(blockTest1);

    Channel* c2 = go(blockTest2);

    Value out = receive(c1);
    out = receive(c2);

    //prevents compiler warning about unused variable
    if (out.asInt) {}


    //mass routine spawning/side effect test

    Channel* mutater = go(sideEffect);

    out = receive(mutater);

    if (out.asInt) {}

    if (mutated != 1000) {
        printf("failed mass coroutine test\n");
    }
    else {
        printf("passed mass coroutine test!\n");
    }

    //test blocking/terminating

    Channel* c3 = channel();
    send(c3, asLong(1));
    if (receive(c3).asLong == 1) {
        printf("this shouldn't print\n");
    }
    else {
        printf("this shouldn't print\n");
    }
    printf("this shouldn't print\n");
}

Value blockTest1() {
    printf("blockTest1 before receiving\n");
    Value x = receive(cGlobal1);
    if (x.asInt) {}
    printf("blockTest1 or blockTest2 tie\n");
    return asInt(0);
}

Value blockTest2() {
    printf("blockTest2 before sending\n");
    send(cGlobal1, asInt(77));
    printf("blockTest1 or blockTest2 tie\n");
    return asInt(0);
}

Value sideEffect() {
    if (mutated < 1000) {
        mutated += 1;
        Channel* left = go(sideEffect);
        Channel* right = go(sideEffect);
        int out = receive(left).asInt;
        out = receive(right).asInt;
        if (out) {}
        if (accumulator++ % 2 == 0) again();
    }
    return asInt(0);
}
