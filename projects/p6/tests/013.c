#include <stdio.h>

#include "src/go.h"

// Objective Tested: if senders are in queue after a return, those senders values are paired off before the return value.

Value testReturnSenders(){
    for(long i = 0; i < 100; i++){
        send(me(), asLong(i));
    }
    return asLong(100000);
}

// Objective Tested: test that yielded function will never execute
Value uselessFunc(){
    send(me(), asString("we <3 gheith"));
    receive(me());
    printf("Error, this shouldn't execute\n");
    return asString("gheith </3 me");
}

int main()
{
    go((Func)&uselessFunc);
    Channel *ch2 = go((Func)&testReturnSenders);
    Channel *out = channel();
    for(int i = 0; i < 100; i++){
        if (receive(ch2).asLong == 100000)
        {
            printf("Error, messed up queue priority\n");
        }
    }
    for (int i = 0; i < 100; i++)
    {
        if (receive(ch2).asLong != 100000)
        {
            printf("Error, messed up infinite return\n");
        }
    }
    printf("Success!\n");
    send(out, asInt(0));
    printf("Shouldn't Print\n");
    return 0;
}
