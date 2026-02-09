#include "debug.h"
#include "shared.h"

struct TestObject {
    int id;

    TestObject(int id) : id(id) {
        Debug::printf("*** Constructing TestObject %d\n", id);
    }

    ~TestObject() {
        Debug::printf("*** Destructing TestObject %d\n", id);
    }
};

void testStrongAndWeakPointers() {
    // Create a strong pointer to a new TestObject
    StrongPtr<TestObject> sp1 = StrongPtr<TestObject>::make(1);
    Debug::printf("*** sp1 points to TestObject %d\n", sp1->id);

    // Create a weak pointer from sp1
    WeakPtr<TestObject> wp1(sp1);
    Debug::printf("*** Created wp1 from sp1\n");

    // Promote wp1 to a strong pointer
    StrongPtr<TestObject> sp2 = wp1.promote();
    if (sp2 == nullptr) {
        Debug::printf("*** sp2 promotion failed\n");
    } else {
        Debug::printf("*** sp2 promotion succeeded, points to TestObject %d\n", sp2->id);
    }

    // Reset sp1
    sp1 = nullptr;
    Debug::printf("*** sp1 reset to nullptr\n");

    // Attempt to promote wp1 again
    StrongPtr<TestObject> sp3 = wp1.promote();
    if (sp3 == nullptr) {
        Debug::printf("*** sp3 promotion failed after sp1 reset\n");
    } else {
        Debug::printf("*** sp3 promotion succeeded, points to TestObject %d\n", sp3->id);
    }

    // Reset sp2
    sp2 = nullptr;
    Debug::printf("*** sp2 reset to nullptr\n");

    // Reset sp2 and sp3
    sp2 = nullptr;
    sp3 = nullptr;
    Debug::printf("*** sp2 and sp3 reset to nullptr\n");

    // Now all strong pointers are gone, object should be deleted
    // Attempt to promote wp1 one last time
    StrongPtr<TestObject> sp4 = wp1.promote();
    if (sp4 == nullptr) {
        Debug::printf("*** sp4 promotion failed as expected, object is deleted\n");
    } else {
        Debug::printf("*** sp4 promotion succeeded unexpectedly\n");
    }

    // Create multiple strong pointers to the same object
    StrongPtr<TestObject> sp5 = StrongPtr<TestObject>::make(2);
    Debug::printf("*** sp5 points to TestObject %d\n", sp5->id);

    StrongPtr<TestObject> sp6 = sp5;
    Debug::printf("*** sp6 copied from sp5, points to TestObject %d\n", sp6->id);

    // Create a weak pointer to the object
    WeakPtr<TestObject> wp2(sp5);

    // Reset one strong pointer
    sp5 = nullptr;
    Debug::printf("*** sp5 reset to nullptr\n");

    // Object should still exist via sp6
    Debug::printf("*** sp6 still points to TestObject %d\n", sp6->id);

    // Promote wp2
    StrongPtr<TestObject> sp7 = wp2.promote();
    if (sp7 == nullptr) {
        Debug::printf("*** sp7 promotion failed\n");
    } else {
        Debug::printf("*** sp7 promotion succeeded, points to TestObject %d\n", sp7->id);
    }

    // Reset remaining strong pointers
    sp6 = nullptr;
    sp7 = nullptr;
    Debug::printf("*** sp6 and sp7 reset to nullptr\n");

    // Now the object should be deleted
    StrongPtr<TestObject> sp8 = wp2.promote();
    if (sp8 == nullptr) {
        Debug::printf("*** sp8 promotion failed as expected, object is deleted\n");
    } else {
        Debug::printf("*** sp8 promotion succeeded unexpectedly\n");
    }

    // Test cyclic references
    struct Node {
        int value;
        StrongPtr<Node> next;

        Node(int val) : value(val) {
            Debug::printf("*** Constructing Node %d\n", value);
        }

        ~Node() {
            Debug::printf("*** Destructing Node %d\n", value);
        }
    };

    // Create nodes
    StrongPtr<Node> node1 = StrongPtr<Node>::make(1);
    StrongPtr<Node> node2 = StrongPtr<Node>::make(2);

    // Create a cycle
    node1->next = node2;
    node2->next = node1;

    // Reset nodes
    node1 = nullptr;
    node2 = nullptr;
    Debug::printf("*** node1 and node2 reset to nullptr\n");
    // Nodes may not be deleted due to cyclic references
    // This demonstrates the limitation of reference counting with cycles
}

void kernelMain(void) {
    Debug::printf("*** Starting strong and weak pointer tests\n");
    testStrongAndWeakPointers();
    Debug::printf("*** Finished strong and weak pointer tests\n");
}