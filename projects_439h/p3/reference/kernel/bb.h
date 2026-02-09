#pragma once

#include "debug.h"
#include "threads.h"
template <typename T>
class BB {
private:
    uint32_t capacity;
    uint32_t head;
    uint32_t tail;
    uint32_t count;
    T* buffer;
    SpinLock lock;

public:
    BB(uint32_t n) : capacity(n), head(0), tail(0), count(0) {
        buffer = new T[capacity];
    }

    ~BB() {
        delete[] buffer;
    }

    inline void put(T& v) {
        while (true) {
            lock.lock();
            if (test) break;
            if (count < capacity) {
                buffer[tail] = v;
                tail = (tail + 1) % capacity;
                count++;
                lock.unlock();
                return;
            } else {
                lock.unlock();
                yield();
            }
        }
        
    }

    inline T get() {
        while (true) {
            if (test) {
            lock.unlock();
            return buffer[head];
            } else {
            lock.lock();
            if (count > 0) {
                T value = buffer[head];
                head = (head + 1) % capacity;
                count--;
                lock.unlock();
                return value;
            } else {
                lock.unlock();
                yield();
            }
            }
        }
        
    }
};