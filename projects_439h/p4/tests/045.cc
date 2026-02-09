#include "threads.h"
#include "debug.h"
#include "shared.h"
#include "barrier.h"
#include "bb.h"

// Dummy object struct
struct Dummy {
    int id; 

    // Constructor
    Dummy(int id) : id(id) {
        Debug::printf("*** Dummy %d created\n", id);
    }

    // Destructor
    ~Dummy() {
        Debug::printf("*** Dummy %d destroyed\n", id);
    }

    // Member function
    void dummyMessage() const {
        Debug::printf("*** Dummy %d message\n", id);
    }
};

void kernelMain() {
    // testing basic strong pointer creation/use/destruction
    StrongPtr<Dummy> s1 = StrongPtr<Dummy>::make(1);
    s1->dummyMessage(); 
    s1 = nullptr;

    // tests strong pointer copying
    StrongPtr<Dummy> s2 = StrongPtr<Dummy>::make(2);
    StrongPtr<Dummy> s3 = s2;
    s2 = nullptr;
    s3->dummyMessage(); 
    s3 = nullptr;

    // tests weak pointer successful promotion
    StrongPtr<Dummy> s4 = StrongPtr<Dummy>::make(3);
    WeakPtr<Dummy> w1(s4);
    StrongPtr<Dummy> wToS1 = w1.promote();
    ASSERT(wToS1 != nullptr);
    // test weak->strong pointer functionality
    wToS1->dummyMessage();
    wToS1 = nullptr;
    s4 = nullptr;

    // tests weak pointer failed promotion
    StrongPtr<Dummy> wToS2 = w1.promote();
    ASSERT(wToS2 == nullptr);

    // test chain weak->strong promotion, all weak->strong promotions should be valid
    StrongPtr<Dummy> s5 = StrongPtr<Dummy>::make(4);
    WeakPtr<Dummy> w2(s5);
    StrongPtr<Dummy> wToS3 = w2.promote();
    // 1 weak pointer, 2 strong pointer
    ASSERT(wToS3 != nullptr);
    s5 = nullptr;

    // 1 weak pointer, 1 strong pointer

    Debug::printf("*** Dummy 4 should still exist\n");
    WeakPtr<Dummy> w3(wToS3);
    StrongPtr<Dummy> wToS4 = w3.promote();
    // 1 weak pointer, 2 strong pointer
    ASSERT(wToS4 != nullptr);
    wToS3 = nullptr;

    // 1 weak pointer, 1 strong pointer
    Debug::printf("*** Dummy 4 should still exist\n");
    wToS4->dummyMessage();
    wToS4 = nullptr;
    // 1 weak pointer, 0 strong pointer
    // now dummy 4 should be deleted



}
