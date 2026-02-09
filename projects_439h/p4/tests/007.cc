#include "stdint.h"
#include "debug.h"
#include "shared.h"
#include "barrier.h"

// This test case verifies a selection of weak pointer functionality, largely based around promotion
// Also, as is tradition, I will be asserting that you do not accidentally disable interrupts!


struct MyStruct {
    int value;

    MyStruct (int valueNew){
        value = valueNew;
    }

    ~MyStruct() {
        //Debug::printf("*** MyStruct deleted with value  %d\n",value);
    }
};

void kernelMain(void) {

    // Part 1: does basic promotion work?
    {
        ASSERT(Interrupts::isDisabled() == false);
        Debug::printf("*** Starting part 1\n");
        StrongPtr<MyStruct> pt1Strong {new MyStruct(1)};
        WeakPtr<MyStruct> pt1Weak {pt1Strong};
        ASSERT(Interrupts::isDisabled() == false);
        
        // Promote the WeakPtr;
        ASSERT(Interrupts::isDisabled() == false);
        StrongPtr<MyStruct> pt1Promoted = pt1Weak.promote();
        ASSERT(pt1Promoted != nullptr);
        ASSERT(pt1Promoted->value == 1);
        ASSERT(Interrupts::isDisabled() == false);
        Debug::printf("*** Part 1 completed. Good Job!\n");
    }

    // Part 2: if the initial strong pointer no longer points to our object, does promotion return null?
    {
        ASSERT(Interrupts::isDisabled() == false);
        Debug::printf("*** Starting part 2\n");
        StrongPtr<MyStruct> pt2Strong {new MyStruct(1)};
        StrongPtr<MyStruct> pt2OtherStrong {new MyStruct(2)};
        WeakPtr<MyStruct> pt2Weak {pt2Strong};
        ASSERT(Interrupts::isDisabled() == false);

        // Remove the StrongPtr
        pt2Strong = pt2OtherStrong;
        
        // Promote the WeakPtr;
        ASSERT(Interrupts::isDisabled() == false);
        StrongPtr<MyStruct> pt2Promoted = pt2Weak.promote();
        ASSERT(pt2Promoted == nullptr);
        Debug::printf("*** Part 2 completed. Fantastic!\n");
        ASSERT(Interrupts::isDisabled() == false);
    }

    // Part 3: if the initial strong pointer no longer points to our object, but there is another strong pointer to our object, promotion should not return null
    {
        ASSERT(Interrupts::isDisabled() == false);
        Debug::printf("*** Starting part 3\n");
        StrongPtr<MyStruct> pt3Strong {new MyStruct(1)};
        StrongPtr<MyStruct> pt3StrongRedunant = pt3Strong;
        StrongPtr<MyStruct> pt3OtherStrong {new MyStruct(2)};
        WeakPtr<MyStruct> pt3Weak {pt3Strong};
        ASSERT(Interrupts::isDisabled() == false);

        // Remove the StrongPtr, leaving another strong pointer still pointing to the 1 struct
        pt3Strong = pt3OtherStrong;
        
        // Promote the WeakPtr;
        ASSERT(Interrupts::isDisabled() == false);
        StrongPtr<MyStruct> pt3Promoted = pt3Weak.promote();
        ASSERT(pt3Promoted != nullptr);
        ASSERT(pt3Promoted->value == 1);
        Debug::printf("*** Part 3 completed. I am so impressed!\n");
        ASSERT(Interrupts::isDisabled() == false);
    }

    // Part 4: No problems should be caused if two weakpointers are promoting at the same time
    {
        ASSERT(Interrupts::isDisabled() == false);
        Debug::printf("*** Starting part 4\n");
        StrongPtr<MyStruct> pt5OriginalStrong {new MyStruct(1)};
        int threadsForPart4 = 11;
        auto pt5Barrier = new Barrier(threadsForPart4+1);

        for (int i=1; i<=threadsForPart4; i++) {
            thread([&pt5OriginalStrong, pt5Barrier] {
                ASSERT(Interrupts::isDisabled() == false);
                WeakPtr<MyStruct> pt5ThreadWeak {pt5OriginalStrong};
                StrongPtr<MyStruct> pt5ThreadPromoted = pt5ThreadWeak.promote();
                ASSERT(pt5ThreadPromoted != nullptr);
                ASSERT(pt5ThreadPromoted->value == 1);
                pt5Barrier->sync();
                ASSERT(Interrupts::isDisabled() == false);

            });
        }

        // Promote the WeakPtr
        WeakPtr<MyStruct> pt5Weak {pt5OriginalStrong};
        StrongPtr<MyStruct> pt5Promoted = pt5Weak.promote();
        ASSERT(pt5Promoted != nullptr);
        ASSERT(pt5Promoted->value == 1);
        pt5Barrier->sync();
        ASSERT(Interrupts::isDisabled() == false);

        Debug::printf("*** Part 4 completed. You are as a god!\n");
        ASSERT(Interrupts::isDisabled() == false);
    }

    // Part 5: To increase our odds of race conditions, we will add deletion and reasignment of strong pointers to that reference
    {
        ASSERT(Interrupts::isDisabled() == false);
        Debug::printf("*** Starting part 5\n");
        StrongPtr<MyStruct> pt5OriginalStrong {new MyStruct(1)};
        int threadsForPart4 = 11;
        auto pt5Barrier = new Barrier(threadsForPart4+1);

        for (int i=1; i<=threadsForPart4; i++) {
            thread([&pt5OriginalStrong, pt5Barrier] {
                ASSERT(Interrupts::isDisabled() == false);
                WeakPtr<MyStruct> pt5ThreadWeak {pt5OriginalStrong};

                // Create another strong pointer to this object, then let it immediately go out of scope
                {
                    StrongPtr<MyStruct> pt5ThreadStrongToGetDeleted = pt5OriginalStrong;
                }

                // Create another strong pointer to this object, then reassign it
                StrongPtr<MyStruct> pt5ThreadStrongToReassignTo {new MyStruct(2)};
                {
                    StrongPtr<MyStruct> pt5ThreadStrongToGetReassigned = pt5OriginalStrong;
                    pt5ThreadStrongToGetReassigned = pt5ThreadStrongToReassignTo;
                }
                StrongPtr<MyStruct> pt5ThreadPromoted = pt5ThreadWeak.promote();
                ASSERT(pt5ThreadPromoted != nullptr);
                ASSERT(pt5ThreadPromoted->value == 1);
                pt5Barrier->sync();
                ASSERT(Interrupts::isDisabled() == false);

            });
        }

        // Promote the WeakPtr
        WeakPtr<MyStruct> pt5Weak {pt5OriginalStrong};
        StrongPtr<MyStruct> pt5Promoted = pt5Weak.promote();
        ASSERT(pt5Promoted != nullptr);
        ASSERT(pt5Promoted->value == 1);
        pt5Barrier->sync();
        ASSERT(Interrupts::isDisabled() == false);

        Debug::printf("*** Part 5 completed. I have nothing more to teach you. Goppert isn't looking, so take this bagel as a prize!\n");
        Debug::printf("*** (Goppert gave his permission for this reference)\n");
        ASSERT(Interrupts::isDisabled() == false);
    }

    
}

