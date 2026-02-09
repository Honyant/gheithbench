#include "debug.h"
#include "shared.h"

struct a {
    int value;
    StrongPtr<a> friend_ptr;

    a(int v) : value(v) {
        Debug::printf("*** a created with value %d\n", v);
    }

    ~a() {}

    void prin() const {
        Debug::printf("*** value: %d\n", value);
    }

};

void kernelMain() {
    //t1
    StrongPtr<a> sp_null;
    Debug::printf("*** t1 - are you properly nullptr?\n");
    Debug::printf("*** sp_null is a nullptr: %s\n", (sp_null == nullptr) ? "true" : "false");

    //t2
    auto sp1 = StrongPtr<a>::make(439);
    Debug::printf("*** t2 - let's give you a value\n");
    sp1->prin();

    //t3
    WeakPtr<a> wp1{sp1};
    Debug::printf("*** t3 - let's make a wkptr from the sptr\n");

    //t4
    StrongPtr<a> sp2 = wp1.promote();
    Debug::printf("*** t4 - how about an upgrade!! wkptr -> sptr, now we have 2 sptrs!\n");
    Debug::printf("*** sp2 is a nullptr: %s\n", (sp2 == nullptr) ? "true" : "false");
    sp2->prin();

    //t5
    StrongPtr<a> sp3 = sp2;
    Debug::printf("*** t5 - let's give the second sptr a twin. now we have 3 sptrs!\n");
    sp3->prin();

    //t6
    sp2 = nullptr;
    Debug::printf("*** t6 - let's just make sure that your wkptr promotion works ok, and that it doesn't rely on sptrs that it shouldn't!\n");
    Debug::printf("*** sp2 is a nullptr: %s\n", (sp2 == nullptr) ? "true" : "false");

    StrongPtr<a> sp4 = wp1.promote();
    Debug::printf("*** sp4 is a nullptr: %s\n", (sp4 == nullptr) ? "true" : "false");

    //t7
    sp1 = nullptr;
    sp3 = nullptr;
    Debug::printf("*** t7 - cool, now let's get rid of all of the other sptrs and try to promote a wkptr...what is the expected outcome?\n");

    StrongPtr<a> sp5 = wp1.promote();
    Debug::printf("*** trying to promote wp1 to sp5...\n");
    Debug::printf("*** sp5 is a nullptr: %s\n", (sp5 == nullptr) ? "true" : "false");

    
    //t8 
    Debug::printf("*** t8 - final one! let's see if your reference counting works with circular references\n");
    StrongPtr<a> sp6 = StrongPtr<a>::make(1);
    StrongPtr<a> sp7 = StrongPtr<a>::make(2);

    //everyone is friends with everyone <3
    sp6->friend_ptr = sp7; 
    sp7->friend_ptr = sp6; 

    sp6 = nullptr;
    sp7 = nullptr;

}

