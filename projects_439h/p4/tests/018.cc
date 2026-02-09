#include "debug.h"
#include "shared.h"
#include "barrier.h"
#include "atomic.h"
#include "bb.h"
#include "promise.h"

struct Blob {
    int v;
    StrongPtr<Blob> best_friend {};
    WeakPtr<Blob> other_friend {StrongPtr<Blob> {}};

    Blob (int v) : v(v) {
        Debug::printf("*** creating blob %d\n", v);
    }

    ~Blob() {
        Debug::printf("*** deleting blob %d\n", v);
    }
    
};

void check(bool b) {
    if(b) Debug::printf("*** :D yay!\n");
    else Debug::printf("*** D: uh oh!\n");
}

Atomic<int> count {0};

void kernelMain(void) {
    Debug::printf("*** hello!\n");

    // basic test that objects are created and deleted correctly, and that
    // promotion works as expected
    StrongPtr<Blob> s1 {new Blob(1)};
    WeakPtr<Blob> w1 {s1};
    {
        StrongPtr<Blob> s2 {new Blob(2)};
        WeakPtr<Blob> w2 {s2};
        StrongPtr<Blob> s3 = w2.promote();
        check(s3->v == 2);
        w1 = w2;
    }
    s1 = w1.promote();
    check(s1 == nullptr);

    // now let's try creating and then deleting a doubly-linked list
    StrongPtr<Blob> head {new Blob(3)};
    StrongPtr<Blob> prev = head;
    for(int i = 4; i < 10; i++) {
        StrongPtr<Blob> a_friend {new Blob(i)};
        prev->best_friend = a_friend;
        a_friend->other_friend = WeakPtr<Blob> {prev};
        prev = a_friend;
    }
    prev = nullptr;
    head = nullptr;

    // now let's try some stuff with shared pointers and a promise!
    WeakPtr<Promise<int>> w3 {StrongPtr<Promise<int>> {}};
    Barrier* b = new Barrier(2);
    {
        StrongPtr<Promise<int>> promise {new Promise<int>()};
        WeakPtr<Promise<int>> w_promise {promise};
        w3 = w_promise;

        thread([w_promise, b] {
            {
                WeakPtr<Promise<int>> w {w_promise};
                w.promote()->set(123);
            }
            b->sync();
        });

        check(promise->get() == 123);
    }
    b->sync();
    check(w3.promote() == nullptr);
    Debug::printf("*** bye!\n");
}