#include "debug.h"
#include "critical.h"
#include "atomic.h"
#include "random.h"
#include "smp.h"

static uint32_t counter = 0;
static uint32_t recursive_counter = 4;
static uint32_t cores_finished = 0;
static Barrier* printBarrier = new Barrier(kConfig.totalProcs);
static int vars[16];
static int small_counter = 0;

/*
This tests a variety of critical calling patterns
1) Calling critical within a critical section should be allowed since we are already the only core
accessing the outer critical section
2) Recrusive critical calling, a slight extension of above, should work fine
3) Calling critical immediately after another critical ensures that your code transitions locks 
correctly and doesn't rely on filler operations between the calls to negotiate locks
*/

//two critical sections interacting

void work2(){
    //find the counter-th prime
    int start = 1;
    if(counter > 0){
        start = vars[counter-1];
    }
    for(int i = start + 1; i < 100000; i++){
        bool found = true;
        for(int j = 2; j <= i/2; j++){
            if(i % j == 0){
                found = false;
                break;
            }
        }
        if(found){
            vars[counter] = i;
            break;
        }
    }
    counter++;
}

void work1() {
    while(counter < 4*(cores_finished+1)){
        critical(work2);
    }
    cores_finished++;

}

//recursive section with base case, should simply just not infinite loop
void work3(){
    if(recursive_counter > 0){
        recursive_counter--;
        critical(work3);
        recursive_counter++;
    }
}

//checks that locks are being unlocked correctly by calling two quick works back to back
void work4(){
    small_counter++;
}

void work5(){
    if(small_counter == 4){
        Debug::printf("*** counter: %d\n", small_counter); //should only print once
        small_counter++;
    }
}

void kernelMain(void) {
    critical(work1);
    
    critical(work3);
    critical(work4);
    critical(work5);

    //check output
    printBarrier->sync();
    if(SMP::me() == 3) {
        for(uint32_t i = 0; i < 16; i++){
            Debug::printf("***Prime %d: %d -> %d\n", i, vars[i], counter);
        }
    }
    
}