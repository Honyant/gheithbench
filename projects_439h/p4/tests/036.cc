#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "shared.h"
#include "barrier.h"

Atomic<uint32_t> recursive_step{0};
Barrier barrier{9};

class BinaryGraphNode {
public:
    WeakPtr<BinaryGraphNode> parent {*(new StrongPtr<BinaryGraphNode>())};
    uint32_t val;
    StrongPtr<BinaryGraphNode> child1{*(new StrongPtr<BinaryGraphNode>())};
    StrongPtr<BinaryGraphNode> child2{*(new StrongPtr<BinaryGraphNode>())};
    BinaryGraphNode(uint32_t const v) {
        val = v;
    }
};

void recursiveInOrderTraversal(StrongPtr<BinaryGraphNode> *my_node, uint32_t n) {
    thread([my_node, n] {
        uint32_t my_n = 0;
        if (n == 3) {
            my_n = 8;
        }
        if (n == 2) {
            my_n = 4;
        }
        else if (n == 1) {
            my_n = 2;
        }
        else if (n == 0) {
            my_n = 1;
        }
        while (recursive_step.get() < (my_n));
        Debug::printf("*** Traversed node %d\n", (*my_node)->val);

        if (!((*my_node)->child1 == nullptr)) {
            recursiveInOrderTraversal(&(*my_node)->child1, n + 1);
        }
        if (!((*my_node)->child2 == nullptr)) {
            recursiveInOrderTraversal(&(*my_node)->child2, n + 1);
        }
        recursive_step.add_fetch(1);
        barrier.sync();
    });
}

/* Called by one core */
void kernelMain(void) {
    // Set up the graph
    /* Strong pointers going down, weak pointers going up
                _ 0 _
               /     \
              1       1
             / \     / \
            2   2   2   2
    */
    uint32_t my_num = 0;
    StrongPtr<BinaryGraphNode> head {new BinaryGraphNode(my_num)};
    head->child1 = new BinaryGraphNode(1);
    head->child2 = new BinaryGraphNode(1);
    WeakPtr<BinaryGraphNode> parent1 {head};
    head->child1->parent = parent1;
    head->child2->parent = parent1;

    head->child1->child1 = new BinaryGraphNode(2);
    head->child1->child2 = new BinaryGraphNode(2);
    WeakPtr<BinaryGraphNode> parent2 {head->child1};
    head->child1->child1->parent = parent2;
    head->child1->child2->parent = parent2;

    head->child2->child1 = new BinaryGraphNode(2);
    head->child2->child2 = new BinaryGraphNode(2);
    WeakPtr<BinaryGraphNode> parent3 {head->child1};
    head->child2->child1->parent = parent3;
    head->child2->child2->parent = parent3;

    // TEST 1: Assigning a pointer to itself
    Debug::printf("*** TEST 1: ");
    head = head; // shouldn't break the code
    if (head == nullptr) {
        Debug::printf("failed\n");
    }
    else {
        Debug::printf("passed\n");
    }
    
    // TEST 2: Checking if a pointer is equal to a promoted weak reference of itself
    Debug::printf("*** TEST 2: ");
    WeakPtr<BinaryGraphNode> weak_ref {head->child1->parent}; // a sprinkle of complexity
    if (head == weak_ref.promote()) {
        Debug::printf("passed\n");
    }
    else {
        Debug::printf("failed\n");
    }

    // TEST 3: Creating a graph cycle and abandoning references to it
    /*          
           2 (this three will null its left child by replacing with a new one)
          /  
         7
        / \
       8   9
    */
    Debug::printf("*** TEST 3: ");
    StrongPtr<BinaryGraphNode> subtree = head->child1->child1;
    subtree->child1 = new BinaryGraphNode(7);
    WeakPtr<BinaryGraphNode> parent4 {subtree};
    subtree->child1->parent = parent4;
    subtree->child1->child1 = new BinaryGraphNode(8);
    subtree->child1->child2 = new BinaryGraphNode(9);
    WeakPtr<BinaryGraphNode> parent5 {subtree->child1};
    subtree->child1->child1->parent = parent5;
    subtree->child1->child2->parent = parent5;

    WeakPtr<BinaryGraphNode> weak_ref1 {subtree->child1};
    WeakPtr<BinaryGraphNode> weak_ref2 {subtree->child1->child1};
    WeakPtr<BinaryGraphNode> weak_ref3 {subtree->child1->child2};

    subtree->child1 = new BinaryGraphNode(3); // a sprinkle of complexity
    if (weak_ref1.promote() == nullptr && weak_ref2.promote() == nullptr && weak_ref3.promote() == nullptr) {
        Debug::printf("passed\n");
    }
    else {
        Debug::printf("failed\n");
    }

    // TEST 4: Recursive traversal
    Debug::printf("*** TEST 4 (below)\n");
    recursiveInOrderTraversal(&head, 0);
    recursive_step.add_fetch(1);
    barrier.sync();
    Debug::printf("*** Did you pass?\n");

    // Debug::shutdown();
}

