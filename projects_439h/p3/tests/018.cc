#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "bb.h"
#include "barrier.h"

constexpr int n = 50;
constexpr int m = 4;
constexpr int x = 50;
Atomic<int> count{0};
Atomic<int> last{0};

// This is a test case for bounded buffers

void kernelMain(void) {
    Debug::printf("*** hello! uwu\n");
    Barrier* barrier = new Barrier(2*m * n + 1);
    
    // check that we can create getter and putter threads for a bb
    // in different orders. check blocking for both get and put
    for(int i = 0; i < n; i++) {
        BB<int>* bb = new BB<int>(m);

        for(int j = 0; j < m; j++) {
            thread([bb, i, barrier] {
                int v = bb->get();
                count.fetch_add(v);
                barrier->sync();
            });
        }

        thread([bb, i] {
            for(int j = 1; j <= 2*m; j++) {
                int k = j + i * 2 * m;
                bb->put(k);
            }
        });

        for(int j = 0; j < m; j++) {
            thread([bb, i, barrier] {
                int v = bb->get();
                count.fetch_add(v);
                barrier->sync();
            });
        }
    }

    barrier->sync();
    int expected = n*m*(n*2*m+1);
    if(count.get() != expected) Debug::printf("*** D: expected %d, got %d\n", expected, count.get());
    Debug::printf("*** part 1 done!\n");

    BB<int>* bb2 = new BB<int>(x);
    Barrier* barrier2 = new Barrier(2);

    // this part makes sure that your bb is fifo
    thread([bb2, barrier2] {
        for(int i = 1; i <= 2*x; i++) {
            int v = bb2->get();
            if(v != i) Debug::printf("*** D: expected %d, got %d\n", i, v);
        }
        barrier2->sync();
    });

    thread([bb2] {
        for(int i = 1; i <= 2*x; i++) {
            bb2->put(i);
        }
    });

    barrier2->sync();
    Debug::printf("*** part 2 done!\n");

    // let's also make sure that you can't exceed the capacity of the bb
    BB<int>* bb3 = new BB<int>(1);
    Barrier* barrier3 = new Barrier(2);

    thread([bb3, barrier3] {
        int a = 1;
        bb3->put(a);
        barrier3->sync();
        Debug::printf("hi\n");
        bb3->put(a);
        Debug::printf("*** D: shouldn't get here!\n");
    });

    barrier3->sync();
    Debug::printf("*** all done!\n");
    Debug::printf("*** yay! goodbye! \\(^o^)/\n");
}