#include "stdint.h"
#include "debug.h"
#include "shared.h"
#include "barrier.h"

/*
This test tests the operators of the Strong and Weak Pointers. It uses assert
statements to determine whether the operations are successful. It also uses
threads to make sure that your pointers are handled correctly when there are
possibly race conditions.
*/

struct Obj {
    int num;
};

void kernelMain(void) {
    // constructor
    StrongPtr<int> ptr0{};
    ASSERT(ptr0 == nullptr);

    int lucky_num = 31;
    StrongPtr<int> ptr1{new int(lucky_num)};
    ASSERT(!(ptr1 == nullptr));

    // copy constructor
    StrongPtr<int> ptr2{ptr1};
    ASSERT(ptr1 == ptr2);
    ptr2 = nullptr;
    
    // same value, different object
    StrongPtr<int> ptr3{new int(lucky_num)};
    ASSERT(!(ptr3 == ptr1)); // should point to a different object

    // strong ptr assignment operator
    ptr3 = ptr1;
    ASSERT(ptr3 == ptr1);
    ptr3 = nullptr;

    // arrow operator (non-primitive type)
    Obj* obj1 = new Obj();
    obj1->num = lucky_num;
    StrongPtr<Obj> objPointer{obj1};
    ASSERT(objPointer->num == lucky_num);

    Obj* tempObj = new Obj();
    tempObj->num = lucky_num + 1;
    StrongPtr<Obj> tempObjPointer{tempObj};
    objPointer = tempObjPointer;
    ASSERT(objPointer->num == lucky_num + 1);

    // weak pointers
    WeakPtr<int> weakPtr1{ptr1};
    WeakPtr<int> weakPtr2{weakPtr1}; // both kinds of constructors
    StrongPtr<int> promotion = weakPtr1.promote();
    ptr1 = nullptr;
    ASSERT(!(promotion == nullptr));
    promotion = nullptr;
    StrongPtr<int> promotion2 = weakPtr2.promote();
    ASSERT(promotion2 == nullptr);

    // test with threads
    Barrier* barrier1 = new Barrier(lucky_num + 1);

    StrongPtr<int> threadStrongPtr{new int(lucky_num)};
    WeakPtr<int> weakPtrOutsideThread{threadStrongPtr};
    for (int i = 0; i < lucky_num; i++) {
        thread([threadStrongPtr, barrier1] {
            WeakPtr<int> threadWeakPtr = threadStrongPtr;
            StrongPtr<int> strongInThread = threadWeakPtr.promote(); // just making more references,
            // ensuring that counting is done correctly
            barrier1->sync();
        }); // all thread instances of StrongPtr should be destructed
    }
    ASSERT(!(threadStrongPtr == nullptr));
    barrier1->sync();
    StrongPtr<int> strongPtrOutsideThread = weakPtrOutsideThread.promote();
    ASSERT(!(strongPtrOutsideThread == nullptr));
    
    Debug::printf("*** tests finished! if you have reached here without an assertion error, you have passed :)\n");

}

