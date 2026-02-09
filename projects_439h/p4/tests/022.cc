#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "shared.h"
#include "promise.h"

struct Thing {
    StrongPtr<Promise<int>> p;

    Thing(StrongPtr<Promise<int>> p): p(p) {

    }

    ~Thing() {
        Debug::printf("*** Thing::~Thing\n");
        p->set(42);
    }
};

/* Called by one core */
void kernelMain(void) {
    Debug::printf("*** hello\n");

    auto p = StrongPtr<Promise<int>>::make();
    auto t = StrongPtr<Thing>::make(p);

    thread([t] {
        Debug::printf("*** in thread\n");
    });

    t = nullptr;

    Debug::printf("*** %d\n", p->get());
}

