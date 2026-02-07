#include "src/go.h"
#include <stdio.h>
#include <inttypes.h>

int total_iterations = 0; 
int again_iterations = 0; 

/*
this test will check:
- go
- send
- recieve
- again
- again on main
- termination with empty ready queue
*/

Value f2() {
    int temp = 1;
    temp-=1;
    return asInt(temp); 
}

//this is just a normal function to print things
void f3() {
    printf("even: %d\n", total_iterations);
}


//increments total_iterations variable
//prints even {2, 4, 6, 8, 10, 12, 14, 16, 18}
Value f1() {
    total_iterations += 1;
    if(total_iterations < 20) {
        if(total_iterations % 2 == 0) {
            f3(); 
            again(); //tests using again() within functions
        } else {
            again();
        }
    }
    receive(me());
    return asInt(500);
}

int main() {
    
    Channel* channel_1 = go(f1);  //tests go with nested functions
    send(channel_1, asLong(1)); //tests send
    again_iterations+=1;
    printf("total iterations = %d\n", total_iterations); //should be 20
    printf("again iterations = %d\n", again_iterations); //should be 1, then 2, then 3

    //testing again on main itself: 
    while(again_iterations < 3) {
        again();
    }
    
    Channel* empty = go(f2);
    send(empty, asInt(1)); //this should terminate (sending to an empty ready queue)
    printf("if you get this output, you are not handling empty channels properly\n");

    return 0;
}
