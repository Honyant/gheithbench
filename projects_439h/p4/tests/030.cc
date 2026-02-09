#include "stdint.h"
#include "debug.h"
#include "shared.h"

// this test focuses on testing smart pointers through a linked list

struct Node{
    int val;
    StrongPtr<Node> next{};
    WeakPtr<Node> prev{StrongPtr<Node>{}};

    Node(int val) : val(val) {}
};

void kernelMain(void) {
    Debug::printf("*** WELCOME TO SANRIO WORLD\n");
    Debug::printf("*** let's count characters :)\n");
    
    StrongPtr<Node> head = StrongPtr<Node>{new Node(1)};
    StrongPtr<Node> lastStrongPtr = head;

    for (int i = 2; i <= 5; i++) {
        StrongPtr<Node> next = StrongPtr<Node>{new Node(i)};
        lastStrongPtr->next = next;
        next->prev = WeakPtr<Node>{lastStrongPtr};
        lastStrongPtr = next;
    }

    // iterate forwards
    StrongPtr<Node> current = head;
    while (current != nullptr) {
        Debug::printf("*** created cinnamoroll #%d\n", current->val);
        current = current->next;
    }

    //iterate backwards
    StrongPtr<Node> tail = lastStrongPtr;
    while (tail != nullptr) {
        Debug::printf("*** backwards cinnamoroll #%d\n", tail->val);
        StrongPtr<Node> prev = tail;
        tail = tail->prev.promote();
    }
}