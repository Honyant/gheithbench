#include "debug.h"
#include "shared.h"

struct Node
{
    int id;
    StrongPtr<Node> next;
    WeakPtr<Node> previous;

    Node(int id) : id(id), previous(WeakPtr<Node>{StrongPtr<Node>{}})
    {
        Debug::printf("*** Node %d created\n", id);
    }

    ~Node()
    {
        Debug::printf("*** Node %d deleted\n", id);
    }
};

template <typename T>
void check(StrongPtr<T> p, bool b)
{
    Debug::printf("*** %s\n", ((p == nullptr) == b) ? "happy" : "sad");
}

void testCircularReferences()
{
    Debug::printf("*** Testing Circular References\n");
    {
        auto node1 = StrongPtr<Node>::make(1);
        check(node1, false);
        {
            auto node2 = StrongPtr<Node>::make(2);
            check(node2, false);

            node1->next = node2;
            node2->previous = node1;

            Debug::printf("*** Node 1's next ID: %d\n", node1->next->id);
            auto prev = node2->previous.promote();
            check(prev, false);
            Debug::printf("*** Node 2's previous ID: %d\n", prev->id);
        }
        check(node1->next, true);
        node1 = nullptr;
    }

    Debug::printf("*** Yay! Finished Circular Reference Test\n");
}

void kernelMain(void)
{
    testCircularReferences();
}
