#include "libc.h"

/*
 * This testcase performs an in-order traversal on a binary search tree, where only child processes can go through recursion
 * So each value from 1-50 should be printed
*/

#define NULL ((void*)0)

typedef struct Node {
    int value;
    struct Node* left;
    struct Node* right;
} Node;

// Function to create a new node with a given value
Node* create_node(int value) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->value = value;
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

// Function to insert a value into the binary tree
Node* insert(Node* root, int value) {
    // If the tree is empty, return a new node
    if (root == NULL) {
        return create_node(value);
    }

    // Otherwise, recur down the tree
    if (value < root->value) {
        root->left = insert(root->left, value);
    } else {
        root->right = insert(root->right, value);
    }
    return root;
}

Node* root = NULL;


void in_order(Node* root) {
    if (root == NULL) {
        return;
    }

    // fork processes for left
    int left_id = fork();
    if(left_id < 0) {
        printf("fork failed\n");
        return;
    } else if(left_id == 0) {
        // child
        in_order(root->left);
        exit(0);
    }
    
    uint32_t status = 42;
    // this will be the parent process now
    wait(left_id, &status); // wait for child to terminate
    printf("*** %d\n", root->value);
    
    // fork process for right
    int right_id = fork();
    if(right_id < 0) {
        printf("fork failed\n");
        return;
    } else if(right_id == 0) {
        // child
        in_order(root->right);
        exit(0);
    }

    wait(right_id, &status);
}


int main(int argc, char** argv) {
    // Create the binary search tree
    for(int i = 1; i <= 50; i ++) {
        root = insert(root, i);
    }

    // perform in-order traversal on the binary search tree
    in_order(root);

    shutdown();
    return 0;
}
