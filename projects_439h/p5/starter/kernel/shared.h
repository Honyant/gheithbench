#pragma once

#include "debug.h"
#include "atomic.h"

namespace impl::shared {
}

template <typename T>
class WeakPtr;

template <typename T>
class StrongPtr {

public:
    // A null pointer
    // example:
    //     StrongPtr p{};
    //     p == nullptr; // --> true
    StrongPtr() {
        MISSING();
    }

    ~StrongPtr() {
        MISSING();
    }

    // Wraps p
    // example:
    //     StrongPtr p{q};
    //     p == q; // -> true
    explicit StrongPtr(T* p) {
        MISSING();
    }

    // Copy constructor
    // example:
    //     StrongPtr p{new ...};
    //     StrongPtr q{p};
    //     p == q; // -> true
    StrongPtr(StrongPtr<T> const& src) {
        MISSING();
    }

    // Assignment operator -- StrongPtr
    // example:
    //    auto p = StrongPtr<Thing>::make(...);
    //    auto q = StrongPtr<Thing>::make(...);
    //    p == q; // false
    //    q = q;  // first object is deleted
    //    p == q; // true
    StrongPtr<T>& operator =(StrongPtr<T> const& rhs) {
        MISSING();
    }

    // Assignment opertor -- raw pointer
    // example:
    //     auto p = StrongPtr<Thing>::make(...);
    //     p = new Thing(...); // first object deleted, p refers to the new object
    StrongPtr<T>& operator =(T* rhs) {
        MISSING();
        return *this;
    }

    // Equality - StringPtr
    bool operator ==(StrongPtr<T>const & rhs) const {
        MISSING();
        return false;
    }

    // Equality -- null
    bool operator ==(nullptr_t rhs) const {
        MISSING();
        return false;
    }

    // arrow operator
    // example:
    //    auto p = StrongPtr<Thing>::make(...);
    //    p->do_something();
    T* operator->() const {
        MISSING();
        return nullptr;
    }

    // forwarding factory
    // example:
    //     auto p = StrongPtr<Thing>::make(...);
    //     /* same as StrongPtr<Thing> p { new Thing(...) }; */
    template <typename... Args>
    static StrongPtr<T> make(Args... args) {
        return StrongPtr<T>{new T(args...)};
    }

    friend class WeakPtr<T>;
};

template <typename T>
class WeakPtr {
public:
    WeakPtr(const StrongPtr<T>& src) {
        MISSING();
    }

    ~WeakPtr() {
        MISSING();
    }

    WeakPtr(WeakPtr<T> const& src) {
        MISSING();
    }

    WeakPtr& operator=(WeakPtr<T> const& rhs) {
        MISSING();
        return *this;
    }

    // the returned StrongPtr refers to:
    //     - the original object iff strong references to it still exist
    //     - nullptr iff strong references to the original object are all gone
    //
    // example:
    //    StrongPtr<Thing> p = StrongPtr<Thing>::make(...);
    //    WeakPtr<Thing> w{p};
    //    w.strong() == nullptr;  // false
    //    p = nullptr;
    //    w.strong() == nullptr;  // true
    StrongPtr<T> promote() const {
        MISSING();
        return {};
    }
};
