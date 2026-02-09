#include "debug.h"
#include "shared.h"

//for testing general weak/strong pointer logic without race conditions

struct TestObject {
    int id;

    TestObject(int id) : id(id) {
        Debug::printf("*** Constructing TestObject %d\n", id);
    }

    ~TestObject() {
        Debug::printf("*** Destructing TestObject %d\n", id);
    }
};

void testSelfAssignment() {
    Debug::printf("*** Testing self-assignment edge cases\n");

    // create strong a
    StrongPtr<TestObject> strongA = StrongPtr<TestObject>::make(2);
    Debug::printf("*** strongA points to TestObject %d\n", strongA->id);

    // a = a
    strongA = strongA;  // Should not do anything
    if (strongA != nullptr) {
        Debug::printf("*** strongA self-assignment did not delete the object, still points to TestObject %d\n", strongA->id);
    } else {
        Debug::printf("*** strongA self-assignment failed, object is null (unexpected)\n");
    }

    // weakA from a
    WeakPtr<TestObject> weakA(strongA);
    Debug::printf("*** weakA created from strongA\n");

    // weakA = weakA
    weakA = weakA;  // Should not do anything
    StrongPtr<TestObject> promotedWeakA = weakA.promote();
    if (promotedWeakA != nullptr) {
        Debug::printf("*** weakA promotion still works, points to TestObject %d\n", promotedWeakA->id);
    } else {
        Debug::printf("*** weakA self-assignment failed, object may have been deleted prematurely\n");
    }

    // Reset strongA 
    strongA = nullptr;
    Debug::printf("*** strongA reset to nullptr\n");

    // object still alive?
    if (promotedWeakA != nullptr) {
        Debug::printf("*** Object is still alive after strongA reset, promotedWeakA points to TestObject %d\n", promotedWeakA->id);
    } else {
        Debug::printf("*** Object was deleted unexpectedly after strongA reset\n");
    }

    // reset promotedWeakA
    promotedWeakA = nullptr;
    Debug::printf("*** promotedWeakA reset to nullptr, object should be deleted now\n");

    // object should be null now
    StrongPtr<TestObject> promotedAgain = weakA.promote();
    if (promotedAgain == nullptr) {
        Debug::printf("*** weakA promotion failed as expected after promotedWeakA reset, object is deleted\n");
    } else {
        Debug::printf("*** weakA promotion succeeded unexpectedly, object should have been deleted\n");
    }

    Debug::printf("*** Self-assignment test completed\n");
}



void test1() {
    Debug::printf("*** Testing weak pointer promotion logic\n");

    //create a
    StrongPtr<TestObject> A = StrongPtr<TestObject>::make(1);
    Debug::printf("*** A points to TestObject %d\n", A->id);

    //create weak b
    WeakPtr<TestObject> B(A);
    Debug::printf("*** B created from A\n");

    //promote b -> c
    StrongPtr<TestObject> C = B.promote();
    if (C != nullptr) {
        Debug::printf("*** B promotion to C succeeded, C points to TestObject %d\n", C->id);
    } else {
        Debug::printf("*** B promotion to C failed\n");
    }

    //reset a
    A = nullptr;
    Debug::printf("*** A reset to nullptr\n");

    //promote b -> d
    StrongPtr<TestObject> D = B.promote();
    if (D != nullptr) {
        Debug::printf("*** B promotion to D succeeded, D points to TestObject %d\n", D->id);
    } else {
        Debug::printf("*** B promotion to D failed (unexpected)\n");
    }

    //check that d is not null
    if (D != nullptr) {
        Debug::printf("*** D is not nullptr, object is still alive as expected\n");
    } else {
        Debug::printf("*** D is nullptr, something went wrong\n");
    }

    //reset c and d
    C = nullptr;
    D = nullptr;
    Debug::printf("*** C and D reset to nullptr, all strong pointers should be gone now\n");

    //chck that e is null
    StrongPtr<TestObject> E = B.promote();
    if (E == nullptr) {
        Debug::printf("*** B promotion to E failed as expected, object has been deleted\n");
    } else {
        Debug::printf("*** B promotion to E succeeded unexpectedly, object should have been deleted\n");
    }

    Debug::printf("*** Test 1 completed\n");
}

void kernelMain(void) {
    test1();
    testSelfAssignment();
}
