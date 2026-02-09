#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "bb.h"
#include "blocking_lock.h"
#include "barrier.h"
#include "promise.h"
#include "shared.h"

class Node {
    public:
        StrongPtr<Node> child1;
        StrongPtr<Node> child2;
        WeakPtr<Node> parent{ StrongPtr<Node>{} };
        int val = -1;

        Node (int v) {
            val = v;
        }
};

/* Called by one core */
void kernelMain(void) {
    StrongPtr<Atomic<int>> counter{ new Atomic<int>{11} };
    StrongPtr<Barrier> sleepBarrier{ new Barrier(501) };

    // this might clog the ready queue if you just yield back to it
    Debug::printf("*** sleep test\n");
    for (int i = 0; i < 500; i++) {
        thread([sleepBarrier]() mutable {
            sleep(5);
            sleepBarrier->sync();
        });
    }

    // test preemption
    Debug::printf("*** preemption test \n");
    for (int i = 0; i < 10; i++) {
        thread([counter]() mutable {
            ASSERT(!Interrupts::isDisabled());
            counter->fetch_add(-1);
            while (counter->get() != 0);
        });
    }

    counter->fetch_add(-1);
    while(counter->get() != 0);

    Debug::printf("*** congrats, u preempt\n");

    // a basic barrier and blocking lock test
    Debug::printf("*** barrier & blocking lock test \n");
    StrongPtr<BlockingLock> lock{ new BlockingLock() };
    StrongPtr<Barrier> barrier1{ new Barrier(21) };

    int count = 0;
    for (int i = 0; i < 20; i++) {
        thread([lock, &count, barrier1]() mutable {
            lock->lock();
            count++;
            lock->unlock();

            barrier1->sync();
        });
    }

    Debug::printf("*** Interrupts & Yield test \n");
    // test interrupt state
    ASSERT(!Interrupts::isDisabled());
    yield();
    ASSERT(!Interrupts::isDisabled());
    Interrupts::disable();
    yield();
    ASSERT(Interrupts::isDisabled());
    Interrupts::restore(false);
    ASSERT(!Interrupts::isDisabled());
    Debug::printf("*** Interrupts & Yield test finished \n");

    barrier1->sync();
    Debug::printf("*** the count is %d\n", count);
    Debug::printf("*** barrier & blocking lock test finished \n");

    Debug::printf("*** bounded buffer test \n");
    StrongPtr<BB<int>> buffer{ new BB<int>(50) };
    StrongPtr<Barrier> barrier2{ new Barrier(3) };
    thread([buffer, barrier2]() mutable {
        for (int i = 0; i < 50; i++) {
            int num = i;
            buffer->put(num);
        }
        barrier2->sync();
    });

    thread([buffer, barrier2]() mutable {
        for (int i = 0; i < 50; i++) {
            Debug::printf("*** %d\n", buffer->get());
        }
        barrier2->sync();
    });
    barrier2->sync();
    Debug::printf("*** bounded buffer test finished \n");

    Debug::printf("*** smart pointers test \n");

    
    StrongPtr<Node> root_ptr{ new Node{0} };
    root_ptr->child1 = new Node{1};
    root_ptr->child2 = new Node{2};
    WeakPtr<Node> weak_root_ptr{root_ptr};
    root_ptr->child1->parent = weak_root_ptr;

    Debug::printf("*** from root:\n");
    Debug::printf("*** root: %d, child1: %d, child2: %d\n", root_ptr->val, root_ptr->child1->val, root_ptr->child2->val);

    StrongPtr<Node> child1 = root_ptr->child1;
    Debug::printf("*** from child1:\n");
    Debug::printf("*** root: %d, child1: %d, child2: %d\n", (child1->parent).promote()->val, (child1->parent).promote()->child2->val, (child1->parent).promote()->child2->val);

    Debug::printf("*** from child1 after destroying strong pointer to root:\n");
    root_ptr = nullptr;
    StrongPtr<Node> promoteResult = (child1->parent).promote();
    Debug::printf("*** promote() %s null and it should be null\n", (promoteResult == nullptr) ? "is" : "is not");

    Debug::printf("*** smart pointers test end \n");


    sleepBarrier->sync();
    Debug::printf("*** end sleep test \n");
    Debug::printf("*** ur done\n");
}