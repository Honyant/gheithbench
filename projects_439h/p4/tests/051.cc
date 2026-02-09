#include "shared.h"
#include "barrier.h"
#include "threads.h"
#include "debug.h"

struct MangoNode {
    StrongPtr<MangoNode> next{};
    WeakPtr<MangoNode> prev{next};
    int val;
    
    MangoNode() {};
    MangoNode(int v) {
        val = v;
    }
};


void kernelMain(void) {
    {   
        // Let's make a simple linked list picture
        MangoNode* second = new MangoNode(3);
        StrongPtr<MangoNode> ptr;
        WeakPtr<MangoNode> second_ptr_wk2{ptr};

        {
            StrongPtr<MangoNode> second_ptr = StrongPtr<MangoNode>(second);
            second_ptr_wk2 = WeakPtr<MangoNode>{second_ptr}; // This weak pointer will live after second has been deleted.
            // Sanity check
            MangoNode* first = new MangoNode(5);
            StrongPtr<MangoNode> first_ptr = StrongPtr<MangoNode>(first);
            Debug::printf("*** Let's start with a null check: %s\n", (first_ptr->next == nullptr) ? "true" : "false");
            
            // Circular pointers
            first_ptr->next = second_ptr;
            second_ptr->prev = WeakPtr<MangoNode>{first_ptr};
            StrongPtr<MangoNode> prev = first_ptr->next->prev.promote();
            if (prev == nullptr) Debug::printf("*** Failed first promotion\n");
            Debug::printf("*** Are accesses/values accurate? %s\n", (first->next->val == 3 && prev->val == 5) ? "true": "false");
        }

        // Is there still a strong pointer to second?? Or did it get deleted when I deleted first?
        ptr = second_ptr_wk2.promote();
        Debug::printf("*** Is the weak pointer promotable? %s\n", (ptr == nullptr) ? "false" : "true");
    }

    // Now let's check that our use barrier use case works, run this multiple times    
    WeakPtr<Barrier> b_wk {StrongPtr<Barrier>{}};
    {
        StrongPtr<Barrier> b = StrongPtr<Barrier>{new Barrier(201)};
        {
            b_wk = WeakPtr<Barrier>{b};
            for (int i = 0; i < 200; i++) {
                thread([b] () mutable {
                    Debug::printf("*** Barrier didn't get deleted!!!\n");
                    b->sync();
                });
            }
        }

        b->sync();
    }
    StrongPtr<Barrier> b_str = b_wk.promote();
    if (b_str == nullptr) {
        Debug::printf("*** Barrier didn't get deleted :(\n");
    } else {
        Debug::printf("*** Barrier was deleted!\n");
    }

}