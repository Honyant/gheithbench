#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "shared.h"

struct Node {
    int value;
    StrongPtr<Node> next{};
    WeakPtr<Node> prev{StrongPtr<Node>{}};

    Node(int value) : value(value) {
    }

    Node(int value, StrongPtr<Node> prev) : value(value) {
        this->prev = WeakPtr<Node>{prev};
    }
};


/* Called by one core */
void kernelMain(void) {
    Debug::printf("*** hi\n");
    StrongPtr<Node> head = StrongPtr<Node>{new Node(0)};
    int N = 100;

    // Create linked list
    StrongPtr<Node> current = StrongPtr<Node>{head};
    for(int i = 1; i < N + 1; i++){
        current->next = StrongPtr<Node>{new Node(i, current)};
        current = StrongPtr<Node>{current->next};
    }

    // Forward Traverse
    current = StrongPtr<Node>{head};
    for(int i = 0; i < N; i++){
        if (current->value != i) {
            Debug::printf("*** Failed forward traversal on item %d\n", i);
        }
        current = StrongPtr<Node>{current->next};
    }

    if (current->value != N) {
        Debug::printf("*** Failed forward traversal on item %d\n", N);
    }

    // Backwards Traverse
    for(int i = N; i > 0; i--){
        if (current->value != i) {
            Debug::printf("*** Failed backward traversal on item %d\n", i);
        }
        current = current->prev.promote();
        if (current == nullptr) {
            Debug::printf("*** Failed to promote on backwards traversal on item %d\n", i);
            stop();
        }
    }

    // Delete test
    current = StrongPtr<Node>{head->next};
    head = nullptr;
    for(int i = 1; i < N + 1; i++){
        StrongPtr<Node> prev = current->prev.promote();
        if (prev == nullptr){
        } else {
            Debug::printf("*** Failed to delete on StrongPtr  on item %d\n", i);
        }
        current = StrongPtr<Node>{current->next};
    }

    Debug::printf("*** bye  >o)\n");   
    Debug::printf("***      (_>\n"); 

}

