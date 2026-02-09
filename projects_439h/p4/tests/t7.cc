#include "stdint.h"
#include "debug.h"
#include "shared.h"

template <typename T>
void check(StrongPtr<T> p, bool b) {
    Debug::printf("*** %s\n",((p == nullptr) == b) ? "happy" : "sad");
}

template <typename T>
void eq(StrongPtr<T> p1, StrongPtr<T> p2) {
    Debug::printf("*** %s\n",(p1 == p2) ? "happy" : "sad");
}

template <typename T>
void ne(StrongPtr<T> p1, StrongPtr<T> p2) {
    Debug::printf("*** %s\n",(p1 != p2) ? "happy" : "sad");
}

struct X1 {
    int msg;

    X1 (int msg) : msg(msg) {}

    ~X1() {
        Debug::printf("*** delete %d\n",msg);
    }
};

StrongPtr<X1> make(int msg) {
    StrongPtr<X1> v { new X1(msg) };
    return v;
}

void kernelMain(void) {
 
    {
        StrongPtr<int> p;
        check(p,true);
    }

    {
        StrongPtr<int> p { new int };
        check(p,false);
    }

    {
        StrongPtr<int> p1 { new int };
        StrongPtr<int> p2;
        check(p1,false);
        check(p2,true);

        p2 = p1;
        check(p1,false);
        check(p2,false);

        p1 = nullptr;
        check(p1,true);
        check(p2,false);

        p1 = p2;
        check(p1,false);
        check(p2,false);

        p2 = nullptr;
        check(p1,false);
        check(p2,true);

        p1 = p2;
        check(p1,true);
        check(p2,true);

        StrongPtr<int> p3 { nullptr };
        check(p3,true);
    }

    {
        X1 *p1 = new X1(1);
        delete p1;

        StrongPtr<X1> p2 { new X1(2) };
        check(p2,false);
        Debug::printf("*** contains %d\n",p2->msg);
    }
        
    {
        StrongPtr<X1> p2;
        check(p2,true);

        {
            StrongPtr<X1> p1 { new X1(3) };
            Debug::printf("*** p1 contains %d\n",p1->msg);
            check(p1,false);
            check(p2,true);
            p2 = p1;
            Debug::printf("*** p2 also contains %d\n",p2->msg);
            check(p1,false);
            check(p2,false);
        }
        check(p2,false);
        Debug::printf("*** p2 still contains %d\n",p2->msg);
    }

    {
        StrongPtr<X1> p1 { new X1(4) };
        StrongPtr<X1> p2 { p1 };

        eq(p1,p2);

        StrongPtr<X1> p3;

        ne(p1,p3);
        ne(p2,p3);

        StrongPtr<X1> p4 { new X1(5) };
        ne(p1,p4);
        ne(p2,p4);
        ne(p3,p4);

        p4 = p2;
        eq(p1,p4);
        eq(p2,p4);
        ne(p3,p4);

        p3 = p1;
        eq(p2,p3);

        p1 = nullptr;
        p2 = nullptr;
        p3 = nullptr;
        eq(p1,p2);
        ne(p1,p4);
        p4 = p1;
        eq(p2,p4);
    }

    {
        Debug::printf("*** playing with functions\n");
        auto p1 = make(7);
        auto p2 = make(7);
        
        check(p1,false);
        check(p2,false);
        ne(p1,p2);

        Debug::printf("*** %s\n", (p1->msg == p2->msg) ? "happy" : "sad");
    }

    {
        Debug::printf("*** weak\n");
        auto sp = StrongPtr<X1>::make(100);
        check(sp, false);
        WeakPtr w1{sp};
        {
            auto m1 = w1.promote();
            check(m1, false);
        }
        sp = nullptr;
        check(sp, true);
        check(w1.promote(), true);
    }
}

