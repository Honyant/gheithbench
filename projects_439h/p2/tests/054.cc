#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"
#include "promise.h"


void barrier_test() {
    auto b1 = new Barrier(2);
    auto b2 = new Barrier(2);
    auto b3 = new Barrier(3);
    auto b4 = new Barrier(4);
    auto b5 = new Barrier(6);
    for(int i = 1; i <= 5; i++){
        thread([i, b1, b2, b3, b4, b5] {
            if (i == 1){
                b1->sync();
                Debug::printf("*** 1\n");
                b2->sync();
                b3->sync();
                b4->sync();
                b5->sync();
            } else if (i == 2){
                b2->sync();
                Debug::printf("*** 2\n");
                b3->sync();
                b4->sync();
                b5->sync();
            } else if (i == 3){
                b3->sync();
                Debug::printf("*** 3\n");
                b4->sync();
                b5->sync();
            } else if (i == 4){
                b4->sync();
                Debug::printf("*** 4\n");
                b5->sync();
            } else if (i == 5) {
                b5->sync();
                Debug::printf("*** 5\n");
            }

        });
    } 

    b1->sync();  
    b5->sync();
}

void promise_test(){
    auto p1 = new Promise<int>();
    auto p2 = new Promise<int>();
    auto p3 = new Promise<int>();
    auto b = new Barrier(4);

    thread([p1, p2, p3, b] {
        p1->set(6);
        int val = p2->get();
        Debug::printf("*** %d\n", val); // prints 7
        p3->set(8);
        b->sync();
    });

    thread([p1, p2, p3, b] {
        int val = p1->get();
        Debug::printf("*** %d\n", val); // prints 6
        p2->set(7);
        b->sync();
    });

    thread([p1, p2, p3, b] {
        int val = p3->get();
        Debug::printf("*** %d\n", val); // prints 8
        b->sync();
    });

    b->sync();

}

auto stop_test(){
    auto b = new Barrier(2);

    thread([b]{
        Debug::printf("*** 9\n");
        b->sync();
        stop();
        Debug::printf("*** AHHHHH\n");
        
    });
    
    return b;
}

void kernelMain(void) {
    Debug::printf("*** hello\n");

    barrier_test();

    promise_test();

    stop_test()->sync();

    Debug::printf("*** goodbye\n");


}

