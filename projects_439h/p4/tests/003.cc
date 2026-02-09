#include "bb.h"
#include "threads.h"
#include "debug.h"
#include "barrier.h"
#include "promise.h"
#include "shared.h"

/*

This test case makes sure pre-emption works while testing basic functionality of strong and weak pointers.

*/
Barrier b{11};
Barrier b2{5};
int counter = 0;
bool test_done = false;

struct CustomStruct {
    bool secret_bool; 

    CustomStruct(bool val) : secret_bool(val) {
        Debug::printf("*** new custom struct\n");
    };
    ~CustomStruct() {
        Debug::printf("*** deconstructed\n");
    } 
};

void kernelMain(void) {
    // testing for preemption
    for(int i = 0; i < 4; i++) {
        thread([]{
            while (!test_done);
        });
    }

    // bb lock test
    auto bb_lock = new BB<int>(1);
    for(int i = 0; i < 10; i++) {
        thread([bb_lock]{
            int t = 0;
            bb_lock->put(t);
            Debug::printf("*** Counter: %d\n", ++counter);
            bb_lock->get();
            b.sync();
        });
    }
    b.sync();

    // passing a bb through a promise lol
    auto promise = new Promise<BB<int>*>();
    auto bb = new BB<int>(1);
    auto handoff_bb = new BB<int>(1);

    for(int i = 0; i < 4; i++) {
        thread([promise, handoff_bb]{
            auto bb = promise->get();
            Debug::printf("*** %d\n", bb->get());
            int t = 0;
            handoff_bb->put(t);
            b2.sync();
        });
    }

    int q = 1111111;
    promise->set(bb);
    for(int i = 0; i < 4; i++) {
        bb->put(q); q *= 2;
        handoff_bb->get();
    }
    b2.sync();

    // pointer tests
    StrongPtr<CustomStruct> sp = StrongPtr<CustomStruct>::make(true); 
    WeakPtr<CustomStruct> wp(sp); 
    Debug::printf("*** Pointers created.\n");
    
    StrongPtr<CustomStruct> promoted_weakptr = wp.promote(); 

    if(promoted_weakptr == nullptr){
        Debug::printf("*** promotion failed\n");
    }

    sp = nullptr;
    
    Debug::printf("*** ptr not gone yet\n");

    promoted_weakptr = nullptr;

    Debug::printf("*** object is gone\n");

    StrongPtr<CustomStruct> new_pointer = wp.promote();

    if(new_pointer == nullptr) {
        Debug::printf("*** object is gone...\n");
    }
    else{
        Debug::printf("*** |||\n");
    }
    
    Debug::printf("*** Done!\n");
    test_done = true;
}
