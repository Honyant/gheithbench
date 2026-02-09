#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "shared.h"

struct Cat {
    int id;

    Cat (int id) : id(id) {
        Debug::printf("*** meow cat %d is born\n", id);
    }

    ~Cat() {
        Debug::printf("*** meow cat %d has died\n", id);
    }
};

/* Called by one core */
void kernelMain(void) {
    StrongPtr<Cat> cat1 = StrongPtr<Cat>::make(1);
    StrongPtr<Cat> cat2 = StrongPtr<Cat>::make(2);
    StrongPtr<Cat> cat3 = StrongPtr<Cat>::make(3);

    StrongPtr<Cat> copycat = cat1;

    if(copycat == cat1) {
        Debug::printf("*** copycat same as cat 1\n");
    }

    WeakPtr<Cat> promotioncat = WeakPtr<Cat>{cat2};
    StrongPtr<Cat> promotedcat = promotioncat.promote();
    if(promotedcat == cat2) {
        Debug::printf("*** promoted cat same as cat 2\n");
    }

    // these should not print destructs

    {
        StrongPtr<Cat> outofscopestrongcat = cat2;
        WeakPtr<Cat> outofscopeweakcat = WeakPtr<Cat>{cat2};
    }

    // should print born and died messages
    {
        StrongPtr<Cat> outofscopestrongcat = StrongPtr<Cat>::make(4);
    }

    // make all cats point to cat3
    thread([&cat1, &cat3] {
        cat1 = cat3;
    });

    thread([&cat2, &cat3] {
        sleep(1);
        cat2 = cat3;
    });
}
