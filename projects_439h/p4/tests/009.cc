#include "threads.h"
#include "debug.h"
#include "shared.h"
#include "barrier.h"
#include "bb.h"

struct Tester {
    int val;
    Tester(int val) : val(val) {
        Debug::printf("*** Tester constructor (%d)\n", val);
    }
    ~Tester() {
        Debug::printf("*** Tester destructor (%d)\n", val);
    }
};

struct TreeNode {
    StrongPtr<TreeNode> left, right;
    WeakPtr<TreeNode> parent;
    StrongPtr<Tester> tester;
    int val;

    // Use a preorder traversal to construct a complete binary tree with `levels` layers
    static StrongPtr<TreeNode> constructTree(StrongPtr<TreeNode> parent, StrongPtr<Tester> t, int& val, int levels) {
        StrongPtr<TreeNode> node {new TreeNode{val++, t, parent}};

        if (levels > 0) {
            node->left = constructTree(node, t, val, levels - 1);
            node->right = constructTree(node, t, val, levels - 1);
        }
        return node;
    }

    TreeNode(int v, StrongPtr<Tester> t, StrongPtr<TreeNode> p) : parent(p), tester(t), val(v) {
        Debug::printf("*** TreeNode constructor (%d)\n", val);
    }

    // Use a preorder traversal to destruct the entire tree
    ~TreeNode() {
        Debug::printf("*** TreeNode destructor (%d)\n", val);
        left = nullptr;
        right = nullptr;
    }
};

// To make i a reference for bb puts
void put(BB<int>* bb, int i) {
    bb->put(i);
}

// To check the result of bb gets
void get(BB<int>* bb, int i) {
    if (int x = bb->get(); x != i) {
        Debug::panic("*** got %d instead of %d\n", x, i);
    }
}

void kernelMain(void) {
    // Bounded buffers for coordinating between threads
    BB<int>* bb1 = new BB<int>(1);
    BB<int>* bb2 = new BB<int>(1);

    // Test 1: Passing a StrongPtr to a thread
    Debug::printf("*** Test 1\n");
    {
        StrongPtr<Tester> s = StrongPtr<Tester>::make(1);
        thread([bb1, &s] {
            {
                auto s2 = s;
                Debug::printf("*** Value: %d\n", s2->val);
            }
            put(bb1, 0);
        });

        get(bb1, 0);
    }
    Debug::printf("*** ----------\n***\n");

    // Test 2: Copying a StrongPtr twice and deleting the old ones, making sure the object remains
    Debug::printf("*** Test 2\n");
    {
        StrongPtr<Tester>* s = new StrongPtr<Tester>{new Tester{2}};
        StrongPtr<Tester>* s2;

        thread([bb1, bb2, s, &s2] {
            {
                Debug::printf("*** Value: %d\n", (*s)->val);
                StrongPtr<Tester> s3 {*s};
                put(bb1, 0);

                get(bb2, 1);
                ASSERT(s3 != nullptr);
                Debug::printf("*** Value: %d\n", s3->val);
                s2 = new StrongPtr<Tester>{s3};
            }
            put(bb1, 2);
        });

        get(bb1, 0);
        delete s;
        put(bb2, 1);

        get(bb1, 2);
        ASSERT(*s2 != nullptr);
        Debug::printf("*** Value: %d\n", (*s2)->val);
        delete s2;
    }
    Debug::printf("*** ----------\n***\n");


    // Test 3: Creating a WeakPtr and destroying its object
    Debug::printf("*** Test 3\n");
    {
        WeakPtr<Tester>* w;
        {
            auto s = StrongPtr<Tester>::make(3);
            w = new WeakPtr<Tester>{s};
            Debug::printf(w->promote() == nullptr ? "*** w is null (bad)\n" : "*** w is not null (good)\n");
        }
        Debug::printf(w->promote() == nullptr ? "*** w is null (good)\n" : "*** w is not null (bad)\n");
        delete w;
    }
    Debug::printf("*** ----------\n***\n");


    // Test 4: Copying a WeakPtr, promoting it, destroying the original StrongPtr, then destroying the promoted one
    Debug::printf("*** Test 4\n");
    {
        auto s = StrongPtr<Tester>::make(4);
        WeakPtr<Tester> w {s};

        thread([bb1, bb2, w] () mutable {
            auto s2 = w.promote();
            Debug::printf("*** Value: %d\n", s2->val);
            put(bb1, 0);

            get(bb2, 1);
            Debug::printf("*** Value: %d\n", s2->val);
            s2 = nullptr;
            put(bb1, 2);

            Debug::printf(w.promote() == nullptr ? "*** w is null (good)\n" : "*** w is not null (bad)\n");
            put(bb1, 3);
        });

        get(bb1, 0);
        s = nullptr;
        put(bb2, 1);

        get(bb1, 2);
        Debug::printf(w.promote() == nullptr ? "*** w is null (good)\n" : "*** w is not null (bad)\n");
        get(bb1, 3);
    }
    Debug::printf("*** ----------\n***\n");


    // Test 5: Create a binary tree where each node points strongly to 2 children and weakly to a parent.
    //         Makes sure that destroying the root destroys the entire tree (and that it plays nice with the WeakPtrs)
    //         They also all have StrongPtrs to a Tester, which should be destroyed last
    Debug::printf("*** Test 5\n");
    {
        // Make a complete binary tree with 3 levels
        int v = 1;
        StrongPtr<TreeNode> root {TreeNode::constructTree(StrongPtr<TreeNode>(nullptr), StrongPtr<Tester>::make(-1), v, 3)};

        Debug::printf("***\n*** tree built!\n***\n");

        // Destroy the whole tree, and at the very end the Tester object should be destroyed
        root = nullptr;

        Debug::printf("***\n*** tree destroyed!\n");
    }
    Debug::printf("*** ----------\n***\n");


    Debug::printf("*** Done! Good job! :D\n");
}
