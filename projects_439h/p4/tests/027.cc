#include "stdint.h"
#include "debug.h"
#include "shared.h"

bool delete_printing_enabled = false;

struct Num {
    uint32_t val;

    Num (uint32_t val) : val(val) {}
    
    ~Num() {
        if (delete_printing_enabled) {
            Debug::printf("*** deleted\n");
        }
    }
};

void kernelMain(void) {
    // test case to make sure that moving
    // strong pointers with the assignment operator works correctly.

    const uint32_t N = 99;
    
    {
        StrongPtr<Num> one { new Num{1} };
        StrongPtr<Num> two { new Num{2} };
        StrongPtr<Num> three { new Num{3} };

        StrongPtr<Num> a{one};
        StrongPtr<Num> b{two};
        StrongPtr<Num> c{three};

        for (uint32_t i = 0; i < N; i++) {
            StrongPtr<Num> temp = a;
            a = b;
            b = c;
            c = temp;
        }

        // make sure that the values match
        Debug::printf("*** %d\n", a->val);
        Debug::printf("*** %d\n", b->val);
        Debug::printf("*** %d\n", c->val);

        delete_printing_enabled = true;
    }
    // all pointers should have gone out of scope here
    // expecting three "deleted" lines to be printed
}

