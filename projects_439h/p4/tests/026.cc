#include "debug.h"
#include "shared.h"

struct Value {
    bool condition;
    int number;
};

void kernelMain(void) {
    Debug::printf("*** Welcome to this test\n");

    //first, a simple promotion test
    int x = 5;
    StrongPtr<int> strong {new int(x)};
    WeakPtr<int> weak {strong};

    //there is a strong reference to x
    StrongPtr<int> promoted = weak.promote();

    if (promoted == nullptr)
        Debug::printf("*** Promotion of pointer incorrectly resulted in nullptr\n");
    else
        Debug::printf("*** Promotion of pointer correctly resulted in non-null pointer\n");

    //next, let's test the assignment operator
    int y = 10;
    int z = 15;
    StrongPtr<int> strong1 {new int(y)};
    StrongPtr<int> strong2 {new int(z)};
    WeakPtr<int> weak1 {strong1};

    //we remove the strong reference to y (tests the assignment operator for a strong pointer):
    strong1 = strong2;

    StrongPtr<int> promoted1 = weak1.promote();

    if (promoted1 == nullptr)
        Debug::printf("*** Promotion of pointer correctly resulted in nullptr\n");
    else
        Debug::printf("*** Promotion of pointer incorrectly resulted in non-null pointer\n");

    
    Value* v1 = new Value();
    v1->condition = true;
    v1->number = 20;

    Value* v2 = new Value();
    v2->condition = false;
    v2->number = 50;

    //tests raw pointer constructor case
    StrongPtr<Value> originalV1Strong {v1};
    WeakPtr<Value> originalV1Weak {originalV1Strong};

    StrongPtr<Value> originalV2Strong {v2};
    WeakPtr<Value> originalV2Weak {originalV2Strong};

    //creating 5 pointers to the struct above
    StrongPtr<Value> strongValues[5];
    for (int i = 0; i < 5; i++) {

        //this tests the strong pointer assignment (we are creating references to the v1 struct in a loop)
        strongValues[i] = originalV1Strong;
        Debug::printf("%ld\n", strongValues[i]->number);
    }

    //we reassign the references to v1 in a loop (strong pointer assignment)
    for (int i = 0; i < 5; i++) {
        Debug::printf("%ld\n", v1->number);
        strongValues[i] = originalV2Strong;
    }

    //we still have a strong reference to v1
    Debug::printf("*** %ld\n", v1->number);

    //after next line, we do not have any strong references to v1
    originalV1Strong = v2;

    StrongPtr<Value> promotionAttempt = originalV1Weak.promote();

    if (promotionAttempt == nullptr)
        Debug::printf("*** Promotion of pointer correctly resulted in nullptr\n");
    else
        Debug::printf("*** Promotion of pointer incorrectly resulted in non-null pointer\n");

    //next, a more rigorous test of atomic reference counting implementation
    Value* v3 = new Value();
    v3->condition = true;
    v3->number = 100;

    Value* v4 = new Value();
    v4->condition = false;
    v4->number = 150;

    //creating a strong and weak pointer to v3
    StrongPtr<Value> originalV3Strong {v3};
    WeakPtr<Value> originalV3Weak {originalV3Strong};

    //creating a strong and weak pointer to v4
    StrongPtr<Value> originalV4Strong {v4};
    WeakPtr<Value> originalV4Weak {originalV4Strong};

    //creating 100 pointers to the struct above
    //tests atomic reference counting at a large scale since references are being gained in first loop and lost in second loop.
    StrongPtr<Value> strongValues2 [100];
    for (int i = 0; i < 100; i++) {

        //strong pointer assignment case
        strongValues2[i] = originalV3Strong;
        Debug::printf("%ld\n", strongValues2[i]->number);
    }

    //let's reassign half of the pointers
    for (int i = 0; i < 50; i++) {
        Debug::printf("%ld\n", strongValues2[i]->number);
        strongValues2[i] = originalV4Strong;
    }

    Debug::printf("*** %ld\n", strongValues2[50]->number);

    //now, we create an extra strong and weak pointer to the v3 struct
    //this tests the atomic reference counting at a larger scale since new references are being created after many reference losses
    StrongPtr<Value> extraV3Strong {originalV3Strong};
    WeakPtr<Value> extraV3Weak {extraV3Strong};

    //let's reassign the remaining half of the pointers
    for (int i = 50; i < 100; i++) {
        Debug::printf("%ld\n", strongValues2[i]->number);
        strongValues2[i] = originalV4Strong;
    }

    //now, we should have two strong and two weak pointers to v3
    //this tests the atomic reference counting mechanism since the result of promoting the weak pointers is enforced

    //promotion attempt with strong pointers present (atomic reference count test)
    StrongPtr<Value> promotionAttempt2 = originalV3Weak.promote();

    if (promotionAttempt2 == nullptr)
        Debug::printf("*** Promotion of pointer incorrectly resulted in nullptr\n");
    else
        Debug::printf("*** Promotion of pointer correctly resulted in non-null pointer\n");

    //let's reassign those strong pointers:

    //exactly these prints should work because of our remaining references
    //pointers are reassigned after the print
    Debug::printf("*** %ld\n", originalV3Strong->number);
    originalV3Strong = originalV4Strong;

    Debug::printf("*** %ld\n", extraV3Strong->number);
    extraV3Strong = originalV4Strong;

    Debug::printf("*** %ld\n", promotionAttempt2->number);
    promotionAttempt2 = originalV4Strong;

    //Now, promotion should fail for the remaining weak pointers
    StrongPtr<Value> promotionAttempt3 = extraV3Weak.promote();
    if (promotionAttempt3 == nullptr)
        Debug::printf("*** Promotion of pointer correctly resulted in nullptr\n");
    else
        Debug::printf("*** Promotion of pointer incorrectly resulted in non-null pointer\n");

    StrongPtr<Value> promotionAttempt4 = originalV3Weak.promote();
    if (promotionAttempt3 == nullptr)
        Debug::printf("*** Promotion of pointer correctly resulted in nullptr\n");
    else
        Debug::printf("*** Promotion of pointer incorrectly resulted in non-null pointer\n");

}