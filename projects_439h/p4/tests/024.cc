#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "smp.h"
#include "barrier.h"
#include "promise.h"
#include "bb.h"
#include "shared.h"

/*helper function for testing shared pointers*/
template <typename T>
void check(StrongPtr<T> p, bool b, int testNum) {
    Debug::printf("*** test %d result = %s\n",testNum,((p == nullptr) == b) ? ":D" : ":(");
}
template <typename T>
void eq(StrongPtr<T> p1, StrongPtr<T> p2, int testNum) {
    Debug::printf("*** test %d result = %s\n",testNum,(p1 == p2) ? ":D" : ":(");
}

/* Called by one core */
void kernelMain(void) {
    //test 1: basic nullptr initialization
    StrongPtr<int> p1 {};
    p1 = nullptr;
    check(p1, true, 1);

    //test 2: basic initialization
    StrongPtr<int> p2 { new int(13) };
    check(p2, false, 2);
    
    //test 3: self assignment
    p2 = p2;
    check(p2, false, 3);
    eq(p2, p2, 3);

    //test 4: copy constructor
    StrongPtr<int> p3 { new int(42) };
    StrongPtr<int> p4 { p3 };
    check(p3, false, 4);
    check(p4, false, 4);
    eq(p3, p4, 4); 

    //test 5: multiple references 
    StrongPtr<int> p5 { new int(62) };
    StrongPtr<int> p6 { p5 };
    StrongPtr<int> p7 { p6 };
    check(p5, false, 5);
    check(p6, false, 5);
    check(p7, false, 5);
    p6 = nullptr;
    check(p5, false, 5);
    check(p6, true, 5);
    check(p7, false, 5);

    //test 6: promoting weak pointer after strong pointer deletion
    StrongPtr<int> sp8 { new int(74) };
    WeakPtr<int> wp8 { sp8 };
    sp8 = nullptr;
    check(wp8.promote(), true, 6);

    //test 7: weakptr assignment
    StrongPtr<int> sp9 { new int(4200) };
    WeakPtr<int> wp9 { sp9 };
    WeakPtr<int> wp10 = wp9;
    check(wp10.promote(), false, 7);
    sp9 = nullptr;
    check(wp9.promote(), true, 7);
    check(wp10.promote(), true, 7);

    //test 8: weakptr after deleting strongptr
    {
        StrongPtr<int> sp11 { new int(4) };
        wp10 = sp11;
    }
    check(wp10.promote(), true, 8);

    //test 9: multiple weakptrs
    StrongPtr<int> sp12 { new int(10) };
    WeakPtr<int> wp11 { sp12 };
    WeakPtr<int> wp12 { wp11 };
    sp12 = nullptr;
    check(wp11.promote(), true, 9);
    check(wp12.promote(), true, 9);

    //test 10: circular references
    struct Dummy {
        StrongPtr<Dummy> ref;
        ~Dummy() {
            Debug::printf("*** deleted dummy\n"); 
        }
    };
    StrongPtr<Dummy> a1 { new Dummy };
    StrongPtr<Dummy> a2 { new Dummy };
    a1->ref = a2;
    a2->ref = a1;
    //should NOT print deletion message
    Debug::printf("*** test 10 result = :D\n");

    //test 11: deleting nullptr
    StrongPtr<int> p13 { new int(14) };
    p13 = nullptr;
    p13 = nullptr;
    check(p13, true, 11);

    //test 12: promoting nullptr
    StrongPtr<int> sp14 = {};
    WeakPtr<int> wp14 { sp14 };
    check(wp14.promote(), true, 12);

    //test 13: deleting only after all strong refs are gone
    StrongPtr<Dummy> p15 { new Dummy };
    StrongPtr<Dummy> p16 { p15 };
    check(p15, false, 13);
    check(p16, false, 13);
    p15 = nullptr;
    //should NOT print deletion message
    check(p15, true, 13);
    check(p16, false, 13);
    p16 = nullptr;
    //should print deletion message
    check(p16, true, 13);

    //test 14: promotion inside lambda
    StrongPtr<int> sp17 { new int(25) };
    check(sp17, false, 14);
    WeakPtr<int> wp17 { sp17 };
    auto lambda = [wp17] () mutable {
        Debug::printf("*** attempting to promote weakptr\n");
        StrongPtr<int> sp18 = wp17.promote();
        if (!(sp18 == nullptr)) {
            Debug::printf("*** successfully promoted weakptr\n");
        } else {
            Debug::printf("*** weakptr was already deleted, so no promotion\n");
        }
        sp18 = nullptr;
    };
    lambda();
    check(wp17.promote(), false, 14);
    sp17 = nullptr;
    lambda();
    check(sp17, true, 14);
    check(wp17.promote(), true, 14);
}