#include "debug.h"
#include "shared.h"
#include "threads.h"
#include "barrier.h"

/*
This test case verifies that reference counting and sleep work correctly together.
We will create StrongPtr and WeakPtr, create threads that sleep for some time,
and check that the reference counting works as expected.
*/

struct Dog {
    int value;

    Dog(int v) : value(v) {
        Debug::printf("*** Dog created with value %d\n", value);
    }

    ~Dog() {
        Debug::printf("*** Dog destroyed with value %d\n", value);
    }
};


void kernelMain(void) {
    // Part 1: Test that WeakPtr promotion fails after StrongPtr is destroyed, even after sleep

    Debug::printf("*** Starting Part 1\n");

    auto strongPtr = StrongPtr<Dog>::make(10);
    WeakPtr<Dog> weakPtr{strongPtr};
    Barrier* b1 = new Barrier(2);
    thread([&weakPtr, b1] {
        sleep(1);
        auto promoted = weakPtr.promote();
        if (promoted == nullptr) {
            Debug::printf("*** WeakPtr promotion failed as expected after StrongPtr destruction\n");
        } else {
            Debug::printf("*** WeakPtr promotion succeeded unexpectedly\n");
        }
        b1->sync();
    });

    //sleep(1); // Wait a bit before destroying StrongPtr
    strongPtr = nullptr; // Destroy StrongPtr
    Debug::printf("*** StrongPtr destroyed\n");

    b1->sync();
    Barrier* b3 = new Barrier(2);

    // Part 2: Test that WeakPtr promotion succeeds if StrongPtr is still alive, even after sleep

    Debug::printf("*** Starting Part 2\n");

    strongPtr = StrongPtr<Dog>::make(20);
    WeakPtr<Dog> weakPtr2{strongPtr};

    thread([&weakPtr2, b3] {
        sleep(2);
        auto promoted = weakPtr2.promote();
        if (promoted != nullptr) {
            Debug::printf("*** WeakPtr promotion succeeded as expected\n");
        } else {
            Debug::printf("*** WeakPtr promotion failed unexpectedly\n");
        }
        promoted = nullptr;
        b3->sync();
    });

    b3->sync();
    strongPtr = nullptr;
    // Part 3: Test multiple threads can promote WeakPtr to StrongPtr

    Debug::printf("*** Starting Part 3\n");

    strongPtr = StrongPtr<Dog>::make(30);
    WeakPtr<Dog> weakPtr3{strongPtr};

    int numThreads = 5;
    auto b6 = new Barrier(numThreads + 1);
    Barrier* finalBar = new Barrier(numThreads + 1);

    for (int i = 0; i < numThreads; i++) {
        thread([&weakPtr3, b6, i, finalBar] {
            {
                sleep(2);
                auto promoted = weakPtr3.promote();
                if (promoted != nullptr) {
                    Debug::printf("*** Thread: WeakPtr promotion succeeded\n");
                } else {
                    Debug::printf("*** Thread: WeakPtr promotion failed\n");
                }
                b6->sync();
            }
            finalBar->sync();
        });
    }

    b6->sync(); // Wait for all threads
    finalBar->sync();
    strongPtr = nullptr; // Destroy StrongPtr
    Debug::printf("*** StrongPtr destroyed in Part 3\n");


    Debug::printf("*** Test completed\n");
}
