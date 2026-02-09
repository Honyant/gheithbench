#include "stdint.h"
#include "debug.h"
#include "shared.h"
#include "barrier.h"

#define NUM_FIBS 40

/*
This test very inefficiently calculates Fibonaccis. This is why we need GPUs!
It tests a few edge case behaviors of strong pointers to make sure you implementation
is bulletproof!
*/

struct Core {
    void (*process)();

    Core(void (*p)()) : process(p) {}
};

Atomic<int32_t> work_done {0};
SpinLock special_sp;
uint64_t fibNums[NUM_FIBS];


int calculate_fib(int i){
    if(i == 0 || i == 1) return 1;
    return fibNums[i-1] + fibNums[i-2];
}

//only allow one core to do work at once
void do_work(){
    special_sp.lock();
    int index = work_done.get();
    int result = calculate_fib(index);
    fibNums[index] = result;
    work_done.fetch_add(1);
    special_sp.unlock();
}


void kernelMain(void) {
    Barrier* b = new Barrier(NUM_FIBS + 1);

    //part 1: testing that you allocate your new promote strong pointers correctly
    //you may fail this if your counts aren't updated in the right orders
    auto strong = StrongPtr<int>::make(10);
    WeakPtr weak{strong}; //both of these now point to wherever strong is
    WeakPtr weak2{strong};
    WeakPtr weak3{strong};
    auto unknown1 = weak.promote();
    ASSERT(!(unknown1 == nullptr)); //we should get here correctly
    strong = nullptr; //delete strong
    auto unknown2 = weak2.promote(); //this should also work correctly since unknown1 is now pointing to strong
    ASSERT(!(unknown2 == nullptr));
    unknown1 = nullptr;
    unknown2 = nullptr; //now no strong pointers should point to strong, so it should get destructed
    auto unknown3 = weak3.promote(); //now, this should fail
    ASSERT(unknown3 == nullptr);
    Debug::printf("*** promote logic testing done! \n");
    
    //part 2: a kind of stress test on your strong pointer implementation
    //now we make sure that your pointer works correctly for non primitive types
    for(int i = 0; i < NUM_FIBS; i++){
        thread([b]{
            StrongPtr<Core> core {new Core{do_work}};
            auto p = core->process;
            p(); //does this get lost?
            b->sync();
        });
    }
    b->sync();

    for(int i = 0; i < NUM_FIBS; i++){
        int num = fibNums[i];
        Debug::printf("*** %d-th fibonacci number: %ld \n", (i+1), num);
    } 
}

