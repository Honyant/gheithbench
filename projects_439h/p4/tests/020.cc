#include "stdint.h"
#include "debug.h"
#include "shared.h"

struct Integer {
    int value;
};

struct Strong {
    StrongPtr<Integer> ptr;
};

struct Weak {
    WeakPtr<Integer> ptr;
};

void kernelMain(void) {

    // Test 1: StrongPtr constructors and assignment
    Debug::printf("*** TEST 1\n");
    {
        StrongPtr<Integer> s1;
        Debug::printf("*** s1 = %s\n", s1 == nullptr ? "nullptr" : "unknown");

        StrongPtr<Integer> s2{new Integer{1}};
        Debug::printf("*** s2 = %d\n", s2->value);

        StrongPtr<Integer> s3{s2};
        Debug::printf("*** s3 = %d\n", s3->value);

        StrongPtr<Integer> s4;
        s4 = new Integer{1};
        Debug::printf("*** s4 = %d\n", s4->value);

        StrongPtr<Integer> s5 = s4;
        Debug::printf("*** s5 = %d\n", s5->value);

    }
    Debug::printf("***\n");


    // Test 2: StrongPtr to StrongPtr
    Debug::printf("*** TEST 2\n");
    {
        StrongPtr<Integer> s{new Integer{2}};

        StrongPtr<Strong> s1{new Strong{s}};
        Debug::printf("*** s1 = %d\n", s1->ptr->value);
    };
    Debug::printf("***\n");
    

    // Test 3: WeakPtr contructors and assignment
    Debug::printf("*** TEST 3\n");
    {
        StrongPtr<Integer> s{new Integer{3}};

        WeakPtr<Integer> w1{s};
        Debug::printf("*** w1 = %d\n", w1.promote()->value);

        WeakPtr<Integer> w2{w1};
        Debug::printf("*** w2 = %d\n", w2.promote()->value);

        WeakPtr<Integer> w3 = w1;
        Debug::printf("*** w3 = %d\n", w3.promote()->value);
    }
    Debug::printf("***\n");


    // Test 4: WeakPtr to StrongPtr
    Debug::printf("*** TEST 4\n");
    {
        StrongPtr<Integer> s1{new Integer{4}};
        StrongPtr<Strong> s2{new Strong{s1}};

        WeakPtr<Strong> w1{s2};
        Debug::printf("*** w1 = %d\n", w1.promote()->ptr->value);
    }
    Debug::printf("***\n");


    // Test 5: WeakPtr to WeakPtr
    Debug::printf("*** TEST 5\n");
    {
        StrongPtr<Integer> s{new Integer{5}};
        WeakPtr<Integer> w{s};

        StrongPtr<Weak> s2{new Weak{w}};
        WeakPtr<Weak> w2{s2};
        Debug::printf("*** w2 = %d\n", w2.promote()->ptr.promote()->value);
    }
    Debug::printf("***\n");


    // Test 6: Promote after reassigning StrongPtr
    Debug::printf("*** TEST 6\n");
    {
        StrongPtr<Integer> s{new Integer{6}};
        WeakPtr<Integer> w1{s};
        Debug::printf("*** w1 = %d\n", w1.promote()->value);

        s = nullptr;
        Debug::printf("*** w1 = %s\n", w1.promote() == nullptr ? "nullptr" : "unknown");
    }
}
