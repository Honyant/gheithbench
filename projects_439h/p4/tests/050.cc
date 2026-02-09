#include "stdint.h"
#include "debug.h"
#include "shared.h"
#include "threads.h"

struct randomStruct {
    int someValue;

    randomStruct(int v) : someValue(v) {}
};

void kernelMain(void) {
    // Functional strong/weak pointer test

    // Default construct should be null
    StrongPtr<int> strong_pointer1{};
    if (strong_pointer1 == nullptr) Debug::printf("*** part 1 pass\n");
    else Debug::printf("*** part 1 fail\n");

    // Assigning it should make it not null
    StrongPtr<int> strong_pointer2{new int(2)};
    if (strong_pointer2 == nullptr) Debug::printf("*** part 2 fail\n");
    else Debug::printf("*** part 2 pass\n");

    // Assigning to another strong pointer should make them the same
    StrongPtr<int> strong_pointer3{strong_pointer2};
    if (strong_pointer2 == strong_pointer3) Debug::printf("*** part 3 pass\n");
    else Debug::printf("*** part 3 fail\n");

    // Same value, but diff object should be different
    StrongPtr<int> strong_pointer4{new int(2)};
    if (strong_pointer4 == strong_pointer3) Debug::printf("*** part 4 fail\n");
    else Debug::printf("*** part 4 pass\n");

    // Assigned strong pointer to another strong pointer in a different way should be same
    strong_pointer1 = strong_pointer3;
    if (strong_pointer1 == strong_pointer3) Debug::printf("*** part 5 pass\n");
    else Debug::printf("*** part 5 fail\n");

    // Creating weak pointer and promoting should be same
    WeakPtr<int> weak_pointer{strong_pointer1};
    StrongPtr<int> strong_pointer5 = weak_pointer.promote();
    if (strong_pointer5 == strong_pointer1) Debug::printf("*** part 6 pass\n");
    else Debug::printf("*** part 6 fail\n");

    // Should be able to construct without a constructor
    StrongPtr<randomStruct> strong_pointer6 = StrongPtr<randomStruct>::make(7);
    if (strong_pointer6 == nullptr) Debug::printf("*** part 7 fail\n");
    else Debug::printf("*** part 7 pass\n");

    // Checks if it is constructed correctly
    Debug::printf("*** strong_pointer6 is %d\n", strong_pointer6->someValue);
}
