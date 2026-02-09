#include "debug.h"
#include "shared.h"

struct Holder {
    int val;    

    Holder(int v) : val(v) {
        Debug::printf("***Created Holder for %d\n", v);
    }

    ~Holder() {
        Debug::printf("***Deleted Holder for %d\n", val);
    }
};

void kernelMain(void) {

    // test various methods of StrongPtr
    StrongPtr<Holder> ptr1 {new Holder(1)};
    StrongPtr<Holder> ptr_copy = ptr1;

    ASSERT(ptr1 == ptr_copy);
    ASSERT(ptr1->val == 1);

    StrongPtr<Holder> ptr_null;

    ASSERT(ptr_null == nullptr);

    ptr_null = new Holder(2);
    ASSERT(ptr_null->val == 2);


    {
        volatile StrongPtr<Holder> ptr3 {new Holder(3)};
        // StrongPtr<Holder> ptr4 {new Holder(4)};

        // ptr_null = ptr4;
    }
}