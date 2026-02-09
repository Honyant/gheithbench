#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "shared.h"
#include "promise.h"

#define PROMISE_COUNT  50

struct ThingButNotGheithsThingIPromise {
    StrongPtr<Promise<int>> p;
    uint64_t id = 0;

    ThingButNotGheithsThingIPromise(StrongPtr<Promise<int>> p, uint64_t id): p(p), id(id) {}

    ~ThingButNotGheithsThingIPromise() {
        if ((id + 1) % PROMISE_COUNT == 0)
            Debug::printf("*** Deleted %d\n", id + 1);
        p->set(id);
    }
};

/* Called by one core */
void kernelMain(void) {
    Debug::printf("*** hello\n");
    auto promises = new StrongPtr<Promise<int>>[PROMISE_COUNT];
    // A version of Gheith's test where you don't have to delete the thread immediately
    // Of course, we still need to make sure you can delete your threads ;)
    for (int i = 0; i < 20; i++) { // Run the test 20 times over. You'll run out of memory if you can't delete all the threads in time.
        for (int j = 0; j < PROMISE_COUNT; j++) {
            promises[j] = StrongPtr<Promise<int>>::make();
            auto t = StrongPtr<ThingButNotGheithsThingIPromise>::make(promises[j], j);
            
            auto tAddr = &t;
            thread([t, tAddr, j] () mutable {
                // A few assertions to make sure things are in order
                ASSERT(tAddr != &t);
                ASSERT(t->id == (uint64_t)j);
                ASSERT(!Interrupts::isDisabled());
                t = nullptr; // Intentionally dereference our StorngPtr to delete the object so we don't have to wait for the thread to be deleted
                ASSERT(t == nullptr);
            });
        }
        for (int j = 0; j < PROMISE_COUNT; j++)
        {
            auto count = promises[j]->get();
            if ((count + 1) % PROMISE_COUNT == 0)
                Debug::printf("*** Got promise %d for the %dth time\n", count + 1, i + 1);
        }
        ASSERT(!Interrupts::isDisabled()); //Checking interrupts again, just in case
    }
    Debug::printf("*** Goodbye :)\n"); //You did it! Congrats!
}

