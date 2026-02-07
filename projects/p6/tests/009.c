#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "src/go.h"

int x = 2;
//test your program's ability to both return and return infinitely, as well as test return types
Value ret1() {
    long x = receive(me()).asLong;
    x = x + 123;
    return asLong(x);
}

Value ret2(){ //test edge case return types
    return asString("check this");
}

Value ret3(){
    return asFunc(ret1);
}


int main() {
    Channel* ch = go(ret1);
    send(ch,asLong(100));
    printf("%ld\n",receive(ch).asLong);
    for(int i = 0; i < 100; i++){
        if(receive(ch).asLong != 223){ //test that we have infinite returns on the channel
            printf("wrong return value");
        }
    }
    printf("main pass: %d\n", (2-x));

    Channel* ch2 = go(ret2);
    Channel* ch3 = go(ret3);

    //now we check that each return type is correct

    if(strcmp(receive(ch2).asString, "check this")){ 
        printf("Wrong type returned, expected string \n");
    }
    if(receive(ch3).asFunc != ret1){
        printf("Wrong type returned, expected function \n");
    }

    if(x > 0){ //test again on main
        x--;
        again();
    }
    printf("You Passed! This should be our last line! \n");
    Channel* stop = channel();

    send(stop,asLong(0)); //should terminate since we are sending to empty channel with no active croutines
    printf("the program should terminate before we get here\n");

    return 0;
}

//test possible helper function names so they don't overlap
void yield(){
    return;
}
void return_handler(){
    return;
}
void run_croutine(){
    return;
}