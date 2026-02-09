#include "debug.h"
#include "shared.h"

struct SharedObject {
    int count;
    
    SharedObject(int count) : count(count) {
        Debug::printf("*** SharedObject %d constructor called\n", count);
    }

    ~SharedObject() {
        Debug::printf("*** SharedObject %d destructor called\n", count);
    }
};

struct CycleObject {
    WeakPtr<CycleObject> next;
    int count;

    CycleObject(const StrongPtr<CycleObject>& src, int count) : next(src), count(count) {
        Debug::printf("*** CycleObject %d constructor called\n", count);
    }

    ~CycleObject() {
        Debug::printf("*** CycleObject %d destructor called\n", count);
    }
    
    bool isNextThere() {
        return !(next.promote() == nullptr);
    }
};

// This test solely tests the shared pointer functionality.

void test() {
    // Test StrongPtr declaration and assignment
    StrongPtr<SharedObject> sp1 = StrongPtr<SharedObject>::make(1);
    Debug::printf("*** sp1 points to new SharedObject %d\n", sp1->count);
    
    // assignment operator - strongptr
    StrongPtr<SharedObject> sp11 = sp1;
    Debug::printf("*** sp11 points to same SharedObject %d\n", sp11->count);
    
    // wrapping constructor
    SharedObject* rawPtr = new SharedObject(2);
    StrongPtr<SharedObject> sp2(rawPtr);
    Debug::printf("*** sp2 points to new SharedObject %d\n", sp2->count);
    
    // copy constructor
    StrongPtr<SharedObject> sp21(sp2);
    Debug::printf("*** sp21 points to same SharedObject %d\n", sp21->count);
    
    // assignment operator - raw pointer
    // SharedObject* rawPtr2 = 
    StrongPtr<SharedObject> sp3{};
    sp3 = new SharedObject(3);
    Debug::printf("*** sp3 points to new SharedObject %d\n", sp3->count);
    
    // Equality Testing - StrongPtr
    Debug::printf("*** sp1 == sp11 is %s\n", sp1 == sp11 ? "true" : "false");
    Debug::printf("*** sp1 == sp2 is %s\n", sp1 == sp2 ? "true" : "false");
    Debug::printf("*** sp2 == sp21 is %s\n", sp2 == sp21 ? "true" : "false");
    Debug::printf("*** sp3 == sp1 is %s\n", sp3 == sp1 ? "true" : "false");
    Debug::printf("*** sp1 == sp1 is %s\n", sp1 == sp1 ? "true" : "false");
    Debug::printf("*** sp1 == nullptr is %s\n", sp1 == nullptr ? "true" : "false");
    
    // deconstruct ptrs
    sp1 = nullptr;
    Debug::printf("*** sp1 reset to nullptr\n");
    sp11 = nullptr;
    Debug::printf("*** sp11 reset to nullptr\n");
    sp2 = nullptr;
    Debug::printf("*** sp2 reset to nullptr\n");
    sp21 = nullptr;
    Debug::printf("*** sp21 reset to nullptr\n");
    sp3 = nullptr;
    Debug::printf("*** sp3 reset to nullptr\n");
    
    // Lets skip weakptr promotion test in favor of a
    // weakptr cycle test
    StrongPtr<CycleObject> cycle1{};
    StrongPtr<CycleObject> cycle2{};
    cycle1 = new CycleObject(cycle1, 1);
    cycle2 = new CycleObject(cycle1, 2);
    Debug::printf("*** cycle2->isNextThere() is %s\n", cycle2->isNextThere() ? "true" : "false");
    cycle1 = nullptr;
    Debug::printf("*** cycle1 reset to nullptr\n");
    Debug::printf("*** cycle2->isNextThere() is %s\n", cycle2->isNextThere() ? "true" : "false");
    cycle2 = nullptr;
    Debug::printf("*** cycle2 reset to nullptr\n");
}

void kernelMain(void) {
    Debug::printf("*** start\n");
    test();
    Debug::printf("*** done\n");
}