#pragma once

class Semaphore;

template <typename T>
class Promise {
private:
    Semaphore* sem;
    T value;

public:
    Promise();
    ~Promise();
    Promise(const Promise&) = delete;
    Promise& operator=(const Promise&) = delete;
    
    void set(const T& v);
    T get();
};

extern template class Promise<int>;
extern template class Promise<bool>;
extern template class Promise<void*>;
extern template class Promise<long>;