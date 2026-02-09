#include "stdint.h"
#include "debug.h"
#include "threads.h"
#include "shared.h"
#include "barrier.h"

struct Dummy {
    int num;
    Dummy(int num) : num(num) {}
    ~Dummy() {
        Debug::printf("*** delete %d\n", num);
    }

    void dummy_print() {
        Debug::printf("*** printing %d\n", num);
    }
};

void kernelMain(void) {
    // basic scope deletion
    {
        StrongPtr<Dummy> single_test{new Dummy(10)};
    }


    // testing basic weak pointers
    {
        StrongPtr<Dummy> weak_test{new Dummy(2)};
        WeakPtr<Dummy> weak{weak_test};
        {
            StrongPtr<Dummy> weak_test2{weak_test};

            StrongPtr<Dummy> strong_from_weak = weak.promote();
            strong_from_weak->dummy_print();
        }

        weak_test = nullptr;
        ASSERT(weak.promote() == nullptr);
    }

    // slight stress test of strong pointers
    uint32_t create = 400;
    {
        StrongPtr<Dummy> dummies[create];
        dummies[0] = StrongPtr<Dummy>::make(create);
        for (uint32_t i = 1; i < create; i++) {
            dummies[i] = dummies[0];
        }

        dummies[0]->dummy_print();
    }

    Debug::printf("*** done\n");
}

