/*
* This Test case is free of thread-based race conditions and tests basic functionality
* of Strong and Weak Pointers along with correctly deallocating objects.
*
* Bonus: It also stress tests that you delete whatever you allocate on the heap to keep track 
* of strong and weak reference counts when both reference counts go down to 0
*/


#include "stdint.h"
#include "debug.h"
#include "shared.h"

constexpr uint32_t N = 330000;

struct Test {
    int num;
    bool value;
    Test (int num, bool value) : num(num), value(value) {}

    ~Test() {
       if (value) {
            Debug::printf("*** Test %d passed\n", num);
       }
    }
};

void kernelMain(void) {
    //Tests 1 & 2: Check destructing the original allocation when strong counts are 0
    StrongPtr<Test> ptr1 {new Test(1, false)};
    {
        //copy
        StrongPtr<Test> ptr2 = ptr1;

        //copying from a copy
        StrongPtr<Test> ptr3 = ptr2;

        //test access
        ptr3->value = true;

        {
            //Spawning weak pointers
            WeakPtr<Test> ptr4 {ptr3};
            WeakPtr<Test> ptr5 {ptr2};
            WeakPtr<Test> ptr6 {ptr4};
            
            //Testing basic promoting
            StrongPtr<Test> ptr7 = ptr6.promote();
            ptr7->num = 2;
        }

        //Object or reference counter shouldn't get deleted if there are 0 weak references
        ptr2->num = 1;
        
        //Adding a weak reference
        WeakPtr<Test> ptr4 {ptr2};

        //Removing strong references
        ptr2 = new Test(2, true);
        ptr3 = ptr2;
        ptr1 = nullptr;

        // 0 strong references for Test 1 at this point (Should destruct and print pass)
        
        //Testing unsuccessful promotion
        StrongPtr<Test> ptr5 = ptr4.promote();
        if (!(ptr5 == nullptr)) {
            Debug::printf("*** Shouldn't have promoted when there were 0 strong references\n");
        }
    }
    //0 Strong references for Test 2 (Should destruct and print pass)

    
    //Test 3: Stress Test to ensure deleting reference counter mechanism when strong and weak counts are 0
    {
        StrongPtr<Test> ptr2 {new Test(3, true)};
        for (uint32_t i = 0; i < N; i++) {
            StrongPtr<Test> ptr3 {new Test(3, false)};
        }
    }
}

