#include "stdint.h"
#include "debug.h"
#include "shared.h"
#include "threads.h"
#include "barrier.h"

// reliance on a single "original" strongptr
// combinations of threads creating weak/strong ptrs, deleting them/going out of scope

#define N 10

struct ABC {
    int val;
    ABC(int v) : val(v) {}
    ~ABC() {
        Debug::printf("*** del %d\n", val);
    }
};

StrongPtr<ABC> make(int msg) {
    StrongPtr<ABC> v { new ABC(msg) };
    return v;
}

void kernelMain(void) {
    {
        Debug::printf("*** hi\n");

        Barrier* b1 = new Barrier(N+1);
        Barrier* b2 = new Barrier(N+1);

        StrongPtr<ABC> og_sp = make(10); // original strong pointer
        WeakPtr<ABC> wp{og_sp};

        for (int i = 0; i < N; ++i) {
            thread([b1, b2, wp] { // creates a copy of the weakptr
                // create strong ptr from weak ptr
                StrongPtr<ABC> sp = wp.promote();
                Debug::printf("*** I am here %d\n", sp->val);    
                sp = nullptr;
                b1->sync();

                b2->sync();

                // all weakptrs should be null now
                if ((wp.promote() == nullptr) == 0) {
                    Debug::printf("*** rip!\n");
                }
            });
        }

        b1->sync();
        
        Debug::printf("*** delete original strong pointer\n");
        og_sp = nullptr;

        b2->sync();


    }
}

