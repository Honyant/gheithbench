#pragma once

#include "debug.h"
#include "semaphore.h"

template <typename T>
class Promise {
private:
Semaphore s{0};
T value;
public:
    void set(T const& v) {
        value = v;
        s.signal();
    }
    T get() {
        s.wait();
        s.signal();
        return value;
    }
};



