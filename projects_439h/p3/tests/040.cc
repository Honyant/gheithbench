#include "debug.h"
#include "threads.h"
#include "blocking_lock.h"
#include "barrier.h"
#include "atomic.h"

/*
attempting to test blocking lock (currently very simply)
does NOT test if you have a separate waiting queue for the lock
can currently pass by yielding if lock is called while locked
ideally don't do that
presumably if your heap is using a blocking lock and working you're good
but if you want to change heap to a spinlock and test it this could be useful
loops with different numbers of threads to use all cores / expose race conditions
have also included some debug print statements at key places so check .raw if you fail
*/

const int totalProcs = kConfig.totalProcs;
const int n = totalProcs * 5;
BlockingLock *theLock = new BlockingLock();
Atomic<int> count {0};
Atomic<int> successes {0};

class BLBarrier //barrier implemented with a blocking lock
{ //don't make this your barrier it's bad

    uint32_t n;
    Atomic<uint32_t> reached = 0;
    BlockingLock lock = BlockingLock();

    public:

    BLBarrier(int32_t const n) : n(n) {
        Debug::printf("barrier constructor before locking\n");
        lock.lock(); //create the barrier and then go on
        Debug::printf("barrier constructor after locking\n");
    }
    BLBarrier (Barrier const &) = delete;
    
    void sync()
    {
        Debug::printf("\ncalled sync\n");
        if (reached.add_fetch(1) == n)
        {
            //all are synced, let them out
            Debug::printf("hooray all synced\n");
        }
        else
        {
            //all haven't reached yet, block via blocking lock
            Debug::printf("not synced yet, reached = %d\n", reached.get());
            lock.lock();
        }
        lock.unlock(); //each thread leaving should unlock for next?
    }
};

void kernelMain(void)
{
    //PART 1

    Debug::printf("*** hi\n");
    Debug::printf("should loop from %d threads to %d threads\n", totalProcs, n);

    //loop from totalProcs to double of it (totalProcs + 1 loops)
    for (int a = totalProcs; a <= n; a++) {
        Debug::printf("starting loop with %d threads\n", a);
        count.set(0);

        //threads are made and threads are (all but one) locked out
        for (int i = 0; i < a; i++) {
            thread([] {
                theLock->lock();
                count.add_fetch(1);
            });
        }


        for (int i = 1; i <= a; i++) {
            while (count.get() != i) yield(); //could preempt but eh let's make sure we're not still running
            theLock->unlock(); //locked threads have to block to let kernelMain run again
            //if they spun the lock would likely never be unlocked
        }

        //all threads should be done running
        if (count.get() == a) {
            /*if this isn't happening some threads are never running
            and kernelMain is never leaving the above for loop
            no *** so it isn't part of expected output
            so the test case can work with different numbers of cores
            */
            Debug::printf("successful with %d threads\n", a);
            successes.add_fetch(1);
        }
        theLock->unlock();
    }

    if (successes.get() == n - totalProcs + 1) {
        Debug::printf("*** success!\n");
    } else {
        Debug::printf("*** something went wrong...\n");
    }

    //PART 2
    BLBarrier blb1 = BLBarrier(n+1);
    BLBarrier blb2 = BLBarrier(n+1);
    BLBarrier blb3 = BLBarrier(n+1);
    BLBarrier *blb4 = new BLBarrier(n+1);
    count.set(0);

    for (int i = 0; i < n; i++) {
        thread([&blb1, &blb2, &blb3, blb4] {
            count.add_fetch(1);
            blb1.sync();
            Debug::printf("thread post blb1 call\n");
            if (count.get() != n+1) {
                Debug::printf("count is %d, should be %d\n", count.get(), n+1);
            }
            blb2.sync();
            Debug::printf("thread post blb2\n");
            count.add_fetch(-1);
            blb3.sync();
            Debug::printf("thread post blb3\n");
            if (count.get() != 0) {
                Debug::printf("count is %d, should be 0\n", count.get());
            }
            blb4->sync();
        });
    }
    count.add_fetch(1);
    blb1.sync();
    Debug::printf("main post blb1\n");
    if (count.get() == n+1){
        Debug::printf("*** part 2 going well (:\n");
    }
    blb2.sync();
    Debug::printf("main post blb2\n");
    count.add_fetch(-1);
    blb3.sync();
    Debug::printf("main post blb3\n");
    if (count.get() == 0) {
        Debug::printf("*** part 2 success!\n");
    }
    blb4->sync();
    Debug::printf("*** goodbye ^-^ \n");
}