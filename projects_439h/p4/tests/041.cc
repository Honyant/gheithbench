#include "stdint.h"
#include "debug.h"
#include "shared.h"
#include "threads.h"
#include "barrier.h"
#include "atomic.h"
#include "promise.h"
#include "bb.h"

SpinLock spinlock;

int threadCount = 10;
Barrier b1{threadCount + 1};

struct IntStruct
{
    uint32_t num;

    IntStruct(uint32_t val) : num(val)
    {
        Debug::printf("*** creating struct with value %d\n", num);
    }

    ~IntStruct()
    {
        Debug::printf("*** deleting struct with value %d\n", num);
    }
};

void threadFunction(StrongPtr<IntStruct> original)
{
    WeakPtr<IntStruct> weak{original};
    StrongPtr<IntStruct> strongCopy = weak.promote();
    spinlock.lock();
    if (strongCopy != nullptr)
    {
        Debug::printf("*** promoted with value %d\n", strongCopy->num);
    }
    else
    {
        Debug::printf("*** promotion failed, no strong pointer available.\n");
    }
    spinlock.unlock();
}

void kernelMain()
{

    Debug::printf("*** TEST 1 begin.\n");
    // Test 1: Basic functionality with StrongPtr and WeakPtr
    {
        StrongPtr<IntStruct> strong{new IntStruct(0)};
        Debug::printf("*** created strong pointer with value %d\n", strong->num);

        WeakPtr<IntStruct> weak{strong};

        // Test promotion, should succeed
        {
            StrongPtr<IntStruct> strongCopy = weak.promote();
            Debug::printf("*** promoted with value %d\n", strongCopy != nullptr ? strongCopy->num : 0);
        }

        strong = nullptr;
        Debug::printf("*** strong pointer reset to nullptr.\n");

        // Test promotion after strong is deleted, should fail
        StrongPtr<IntStruct> strongCopy = weak.promote();
        Debug::printf("*** weak pointer promotion after reset: %s\n", strongCopy == nullptr ? "failed" : "succeeded");
    }

    Debug::printf("*** TEST 1 completed.\n");

    Debug::printf("*** TEST 2 begin.\n");

    // Test 2: Barrier Pointers (tests that we can have a pointer of a barrier)
    StrongPtr<Barrier> temp_barrier{NULL};
    WeakPtr<Barrier> weak_barrier{temp_barrier};
    {
        StrongPtr<Barrier> barrier{new Barrier(threadCount + 1)};
        weak_barrier = barrier;
        for (int i = 0; i < threadCount; i++)
        {
            thread([barrier]() mutable
                {
                    Debug::printf("*** running thread\n");
                    barrier->sync();
                });
        }
        barrier->sync();
        Debug::printf("*** barrier is %s\n", weak_barrier.promote() == nullptr ? "null" : "not null");
    }
   
    Debug::printf("*** TEST 2 completed.\n");
    
    Debug::printf("*** TEST 3 begin.\n");

    // Test 3: Promised Pointers (tests that we can have a pointer of a promise)

    StrongPtr<Promise<uint32_t>> promise{new Promise<uint32_t>()};
    WeakPtr<Promise<uint32_t>> weakPromise{promise};

    thread([weakPromise]() mutable
           {
               StrongPtr<Promise<uint32_t>> strongPromise = weakPromise.promote();
               if (strongPromise != nullptr)
               {
                   Debug::printf("*** promise is not null.\n");
               }
               else
               {
                   Debug::printf("*** promise is null.\n");
               }
               strongPromise->set(10000);
           });

    uint32_t value = promise->get();
    promise = nullptr;
    Debug::printf("*** promise value: %d\n", value);


    Debug::printf("*** TEST 3 completed.\n");
    
    
}
