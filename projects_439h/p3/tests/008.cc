#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "bb.h"
#include "barrier.h"

/*
This testcase is useful for validating your implementating of bounded buffer with interrupts on.
It contains some simple tests to check basic logic as well as attempting to poke holes with a few
edge case tests. Overall, it is not a stress test but is rather designed to help you with your coding.
I do stress that the order of values matters, meaning certain implementations may see race conditions.

Each section can be removed without an effect to any of the later sections to isolate your bugs. Assertions
are sprinkled so you exit upon first fail.
*/

extern void work1(){
    Debug::printf("*** successfully called work! \n");
}

struct Func{
    void (*func)();
};

void kernelMain(void) {
    int n1_2 = 50;
    int n3 = 10;
    auto bb = new BB<int>(n1_2-1); //introduce some buffering conflicts
    auto bb2 = new BB<struct Func>(n3-1);
    auto bb3 = new BB<int>(1);
    Barrier* b1 = new Barrier(n1_2+1); 
    Barrier* b2 = new Barrier(2);
    Barrier* b3 = new Barrier(n3+1);
    Barrier* b4 = new Barrier(5);
    Barrier* b5 = new Barrier(4);

    //testing different orders of calling: get before put, checking value is preserved

    for(int i = 0; i < n1_2; i++){
        thread([bb, b1, n1_2] {
            int x = bb->get();
            ASSERT(x == n1_2);  //we should receive the right value
            b1->sync();
        });  
        bb->put(n1_2);  
    }  
    b1->sync(); 

    Debug::printf("*** part 1 done \n");

    //put before get, checking value is preserved

    thread([bb, b2, n1_2] {
        for(int i = 0; i < n1_2; i++){
            bb->put(i);
        }
        b2->sync();
    });

    for(int i = 0; i < n1_2; i++){
        int x = bb->get();  
        Debug::printf("*** buffer got %d, order should be preserved \n", x); 
    }  
    b2->sync(); 

    Debug::printf("*** test 2 done! \n");

    //test 3, make sure that we are able to store more than primitives, i.e. functions
    //this makes sure you don't store things by value alone

    for(int i = 0; i < n3; i++){
        thread([bb2, b3] {
            auto x = bb2->get();
            x.func();
            b3->sync();
        });
        Func f;  
        f.func = &work1;
        bb2->put(f);  //this may go out of scope if you're not careful with how you store the values
    }  
    b3->sync(); 

    Debug::printf("*** test 3 done! \n");

    //test 4, calling get in between puts (the value should not be preserved)
    //first we put a value 
    thread([bb3, b4, b5] {
        int x = 10;
        bb3->put(x);
        b5->sync();
        b4->sync();
        
    });

    //then we get a value
    thread([bb3, b4, b5] {
        int x = bb3->get();
        ASSERT(x==10);
        Debug::printf("*** received value %d \n", x);
        b5->sync();
        b4->sync();
        
    });

    //the we get again, this should not return the same value (not a promise)
    thread([bb3, b4, b5] {
        b5->sync(); //make sure this thread doesn't jump the line
        int x = bb3->get();
        ASSERT(x==20);
        Debug::printf("*** received value %d \n", x);
        b4->sync();
    });

    //put the value back to resolve the conflict
    thread([bb3, b4, b5] {
        b5->sync();
        int y = 20;
        bb3->put(y);
        b4->sync();
    });
    b4->sync();

    Debug::printf("*** test 4 done! \n");
    Debug::printf("*** all tests passed! \n");

}