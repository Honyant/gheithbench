#include "promise.h"
#include "semaphore.h"

template <typename T>
Promise<T>::Promise() : sem(new Semaphore(0)), value() {}

template <typename T>
Promise<T>::~Promise() {
    delete sem;
}

template <typename T>
void Promise<T>::set(const T& v) {
    value = v;
    sem->up();
}

template <typename T>
T Promise<T>::get() {
    sem->down();
    sem->up();
    return value;
}

template class Promise<int>;
template class Promise<bool>;
template class Promise<void*>;
template class Promise<long>;