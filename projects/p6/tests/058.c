//Testing to see if you can handle 2 coroutines and context switching

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "src/go.h"

static int fun11count = 0;
static int fun12count = 0;
static const int maxamt = 12;
static Channel* ch2;
static Channel* ch1;

Value fun12() {
    printf("12\n");
    send(ch2, asInt(fun12count)); //Blocks current channel, switches to ch2
    send(me(), asInt(fun12count)); //Switches momentarily to ch2 before switching back
    fun12count++;
    return asLong(0);
}

Value fun11() {
    while(fun11count < maxamt){
        fun11count++;
        fun12();
    }
    int i = receive(me()).asInt;
    printf("11 %i\n", i); //Should never print
    return asLong(0);
}


Value fun21() {
    for(int i = 0; i < maxamt; i++) {
        int j = receive(me()).asInt; //Should switch to ch1
        printf("21 %i\n", j);
        receive(ch1);
    }
    return asLong(0);
}

int main() {
    ch2 = go(fun21);
    ch1 = go(fun11);
    printf("Two channel test\n");
    send(me(), asInt(0));
    printf("done\n"); //Shouldn't print this
    fflush(stdout);
    return 0;
}