#include "debug.h"
#include "shared.h"

struct X {
    int value;

    X(int val) : value(val) {
        Debug::printf("*** X(%d) constructed\n", value);
    }

    ~X() {
        //Debug::printf("*** X(%d) destroyed\n", value);
    }
};

void kernelMain(void) {
    // create a StrongPtr to an instance of X
    StrongPtr<X> sp{new X(10)};
    Debug::printf("*** StrongPtr created with value %d\n", sp->value);

    // create a WeakPtr from the StrongPtr
    WeakPtr<X> wp{sp};
    Debug::printf("*** WeakPtr created from StrongPtr\n");

    // promote WeakPtr to StrongPtr
    StrongPtr<X> sp2 = wp.promote();
    if (sp2 != nullptr) {
        Debug::printf("*** WeakPtr successfully promoted to StrongPtr with value %d\n", sp2->value);
    } else {
        Debug::printf("*** WeakPtr promotion failed\n");
    }

    // reset the original StrongPtr
    sp = nullptr;
    Debug::printf("*** Original StrongPtr reset\n");

    // //try to promote the WeakPtr again
    // StrongPtr<X> sp3 = wp.promote();
    // if(sp3 != nullptr){
    //     Debug::printf("*** WeakPtr promotion succeeded but it's not supposed to\n");
    // }else{
    //     Debug::printf("*** WeakPtr promotion failed as expected (object destroyed)\n");
    // }

    // at the end, sp2 should still hold a StrongPtr to X(10)
    Debug::printf("*** sp2 still holds value %d\n", sp2->value);

    // reset sp2
    sp2 = nullptr;
    Debug::printf("*** sp2 reset\n");
}
