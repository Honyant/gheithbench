#pragma once

#include "debug.h"
#include "atomic.h"
#include "threads.h"
#include "semaphore.h"

template <typename T>
class BB {
private:
    T* buffer;
    uint32_t size;
    uint32_t head;
    uint32_t tail;
    Semaphore emptySlots;
    Semaphore fullSlots;
    SpinLock bufferLock;

public:
    BB(uint32_t n): buffer(new T[n]), size(n), head(0), tail(0), emptySlots(n), fullSlots(0) {}

    ~BB() {
        delete[] buffer;
    }

    void put(T& v) {
        emptySlots.wait();
        bufferLock.lock();
        buffer[tail] = v;
        tail = (tail + 1) % size;
        bufferLock.unlock();
        fullSlots.signal();
    }

    T get() {
        fullSlots.wait();
        bufferLock.lock();
        T value = buffer[head];
        head = (head + 1) % size;
        bufferLock.unlock();
        emptySlots.signal();
        return value;
    }
};
