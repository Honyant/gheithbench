#include "atomic.h"
#include "critical.h"
#include "debug.h"
#include "smp.h"

struct QueueNode {
    QueueNode *next;
    uint32_t data;
};

class Queue {
    QueueNode *head;
    QueueNode *tail;

  public:
    Queue() : head(nullptr), tail(nullptr) {}

    QueueNode *get_head() { return head; }

    void enqueue(uint32_t data) {
        QueueNode *node = new QueueNode();
        node->data = data;
        node->next = nullptr;
        if (tail) {
            tail->next = node;
            tail = node;
        } else {
            head = tail = node;
        }
    }

    uint32_t dequeue() {
        if (!head) {
            Debug::printf("*** FAILURE: QUEUE IS EMPTY ***\n"); // if test case runs successfully, this line should not ever be printed
            return -1;
        }

        uint32_t data = head->data;
        QueueNode *node = head;
        head = head->next;
        if (!head) {
            tail = nullptr;
        }
        delete node;
        return data;
    }

    bool is_empty() { return head == nullptr; }
};

static uint32_t counter = 0;
static Queue q;

void print_queue() {
    QueueNode *node = q.get_head();
    Debug::printf("*** Queue: ");
    while (node) {
        Debug::printf("%d ", node->data);
        node = node->next;
    }
    Debug::printf("\n");
}

void kernelMain(void) {
    critical([] {
        Debug::printf("*** hello %d\n", counter);
        q.enqueue(counter);
        q.enqueue(counter * counter + 1);
        print_queue();

        while (!q.is_empty()) {
            Debug::printf("*** Dequeued: %d\n", q.dequeue());
            print_queue();
        }

        Debug::printf("*** goodbye %d\n", counter);
        counter++;
    });
}
