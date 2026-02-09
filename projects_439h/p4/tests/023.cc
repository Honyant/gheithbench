#include "debug.h"
#include "threads.h"
#include "shared.h"
#include "barrier.h"
#include "heap.h"

/*
* There is one race condition with shared pointers. This tc tests for that. 
* When you promote a weak pointer, checking the value of the counter for strong pointers and adding 1 to it if it's greater than 0 should be atomic. 
* should fail about 10-50% of the time if you have race conditions? depending on how bad it is.
*/

// i rly tried. couldn't increase the constraints more than this
#define MAX_N 5
#define MAX_K 2


template <typename T>
class PromoteWeakPtr {
    WeakPtr<T> ptr;
    Barrier* barrier_ptr;
    int i;
public:
    PromoteWeakPtr(WeakPtr<T> ptr, Barrier* barrier_ptr, int i) 
        : ptr(ptr), barrier_ptr(barrier_ptr), i(i) {};

    void operator() () {
        StrongPtr<int> promoted_ptr = ptr.promote();
        if (promoted_ptr == nullptr) {
            barrier_ptr->sync();
            return;
        }
        int* cur_array = promoted_ptr.operator->();

        for (int j = 0; j < MAX_N; j++) {
            if (cur_array[j] != i) {
                Debug::printf("*** Memory corruption\n");
                Debug::shutdown();
            }
        }
        barrier_ptr->sync();
    }

    PromoteWeakPtr(const PromoteWeakPtr<T>& original) 
        : ptr(original.ptr), barrier_ptr(original.barrier_ptr), i(original.i) {
    }
};


void kernelMain() {
    for (int i = 0; i < MAX_K; i++) {
        Barrier* barrier_ptr = new Barrier(2);
        thread([barrier_ptr, i] {
            StrongPtr<int> strong_ptr = StrongPtr<int>((int*)malloc(sizeof(int) * MAX_N));
            int* cur_array = strong_ptr.operator->();
            for (int j = 0; j < MAX_N; j++) {
                cur_array[j] = i;
            }
            WeakPtr<int> weak_ptr{strong_ptr};

            thread(PromoteWeakPtr<int> (weak_ptr, barrier_ptr, i));
        });
        barrier_ptr->sync();
    }


    Debug::printf("*** Hey Jude!\n");
}