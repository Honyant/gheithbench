#include "stdint.h"
#include "debug.h"
#include "shared.h"

struct Node {
    int value;
    StrongPtr<Node> left;
    StrongPtr<Node> right;

    Node(int v) : value(v), left(nullptr), right(nullptr) {
        Debug::printf("*** Node created\n");
    }

    ~Node() {
        Debug::printf("*** Node destroyed\n");
    }
};

// recursive function to insert a value into the binary tree
StrongPtr<Node> insert(StrongPtr<Node> root, int value) {
    if (root == nullptr) {
        return StrongPtr<Node>::make(value); 
    }

    if (value < root->value) {
        root->left = insert(root->left, value);
    } else if (value > root->value) {
        root->right = insert(root->right, value);
    }

    return root;
}

// recursive function to print function
void printInOrder(StrongPtr<Node> root) {
    if (!(root == nullptr)) {
        printInOrder(root->left);
        Debug::printf("%d ", root->value);
        printInOrder(root->right);
    }
}

void kernelMain(void) {
    StrongPtr<Node> root;

    // test 1
    Debug::printf("*** Creating the tree: \n");

    for (int i = 1; i <= 20; ++i) {
        root = insert(root, i);
    }

    Debug::printf("*** In-order traversal: ");
    printInOrder(root);
    Debug::printf("\n");

    Debug::printf("*** Deleting the tree\n");
    root = nullptr;

    // test 2
    Debug::printf("*** Creating the tree: \n");

    for (int i = 1; i <= 20; ++i) {
        root = insert(root, i);
    }

    Debug::printf("*** In-order traversal: ");
    printInOrder(root);
    Debug::printf("\n");

    Debug::printf("*** Deleting part of the tree\n");
    root->right->right = nullptr;

    Debug::printf("*** In-order traversal: ");
    printInOrder(root);
    Debug::printf("\n");
}