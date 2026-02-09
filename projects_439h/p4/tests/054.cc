#include "stdint.h"
#include "debug.h"
#include "shared.h"
#include "threads.h"
#include "barrier.h"

struct myStruct {
    int val; 

    myStruct(int v) : val(v) {}
    ~myStruct() {}
};

void kernelMain(void) {
    // first i'll test basic functionality of the pointer
    StrongPtr<myStruct> test0strong {new myStruct(5)};
    if (test0strong->val == 5) {} 
    else {
        Debug::printf("*** there is an error in part 0\n");
    }

    test0strong->val = 10;
    if (test0strong->val == 10){
        Debug::printf("*** passed part 0\n");
    } else {
        Debug::printf("*** there is an error in part 0\n");
    }

    // next i'll test basic counting of strong and weak pointers. 
    StrongPtr<myStruct> test1strong { new myStruct(5) };
    WeakPtr<myStruct> test1weak { test1strong };

    for (int i = 0; i < 10; i++){
        StrongPtr<myStruct> shouldGoAwayAfterForLoop {test1strong}; // should be added to count and then decremented from count
    }

    WeakPtr<myStruct> test1weak1 {test1strong}; // should not influence the count
    WeakPtr<myStruct> test1weak2 {test1strong};

    test1strong = nullptr;

    if (test1weak.promote() == nullptr && test1weak1.promote() == nullptr && test1weak2.promote() == nullptr) {
        Debug::printf("*** passed part 1\n");
    } else {
        Debug::printf("*** a weak pointer was promoted when it shouldn't be!\n");
    }

    // next i will test promotion
    StrongPtr<myStruct> test2strong { new myStruct(15) };
    WeakPtr<myStruct> test2weak {test2strong};
    test2weak.promote()->val = 14;
    if (test2strong->val != 14){
        Debug::printf("*** there is an issue in the promotion test\n");
    }
    test2strong = nullptr;
    if (test2weak.promote() == nullptr) {
        Debug::printf("*** passed part 2\n");
    }

    // finally i ensure strong and weak pointers work properly with threads and barriers
    int numThreads = 200;
    StrongPtr<myStruct> test3strong {};
    Barrier* b = new Barrier(numThreads+1);
    Atomic<int>* count = new Atomic<int>(0);
    for (int i = 0; i < numThreads; i++){
        thread([i, b, count] {
            StrongPtr<myStruct> p {new myStruct(15)};
            (*count).fetch_add(1);
            b->sync();
        });
    }
    b->sync();
    if ((*count).get() == 200){
        Debug::printf("*** passed part 3\n");
    }
}