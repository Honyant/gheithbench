#pragma once

#include "atomic.h"
#include "debug.h"
#include "shared.h"

namespace impl::shared {

template <typename T>
struct ctrlBlock {
    T* ptr;
    int strong_count;
    int weak_count;
    SpinLock lock;

    ctrlBlock(T* p) : ptr(p), strong_count(1), weak_count(0) {}
};

}

template <typename T>
class WeakPtr;

template <typename T>
class StrongPtr {
   public:
    StrongPtr() : ctrl(nullptr) {
    }

    ~StrongPtr() { reset(); }

    explicit StrongPtr(T* p) {
        if (p) {
            ctrl = new impl::shared::ctrlBlock<T>(p);
        } else {
            ctrl = nullptr;
        }
    }

    StrongPtr(const StrongPtr<T>& src) {
        ctrl = src.ctrl;
        if (ctrl) {
            ctrl->lock.lock();
            ctrl->strong_count++;
            ctrl->lock.unlock();
        }
    }

    StrongPtr<T>& operator=(const StrongPtr<T>& rhs) {
        if (this != &rhs) {
            auto old_ctrl = ctrl;
            ctrl = rhs.ctrl;
            if (ctrl) {
                ctrl->lock.lock();
                ctrl->strong_count++;
                ctrl->lock.unlock();
            }
            if (old_ctrl) {
                old_ctrl->lock.lock();
                old_ctrl->strong_count--;
                bool should_delete = (old_ctrl->strong_count == 0);
                old_ctrl->lock.unlock();
                if (should_delete) {
                    delete old_ctrl->ptr;
                    if (old_ctrl->weak_count == 0) {
                        delete old_ctrl;
                    }
                }
            }
        }
        return *this;
    }

    StrongPtr<T>& operator=(T* rhs) {
        auto old_ctrl = ctrl;
        if (rhs) {
            ctrl = new impl::shared::ctrlBlock<T>(rhs);
        } else {
            ctrl = nullptr;
        }
        if (old_ctrl) {
            old_ctrl->lock.lock();
            old_ctrl->strong_count--;
            bool should_delete = (old_ctrl->strong_count == 0);
            old_ctrl->lock.unlock();
            if (should_delete) {
                delete old_ctrl->ptr;
                if (old_ctrl->weak_count == 0) {
                    delete old_ctrl;
                }
            }
        }
        return *this;
    }

    bool operator==(const StrongPtr<T>& rhs) const {
        return get() == rhs.get();
    }

    bool operator==(nullptr_t) const { return get() == nullptr; }

    T* operator->() const { return get(); }

    template <typename... Args>
    static StrongPtr<T> make(Args... args) {
        return StrongPtr<T>{new T(args...)};
    }

    friend class WeakPtr<T>;

    private:
    impl::shared::ctrlBlock<T>* ctrl;

    T* get() const { return ctrl ? ctrl->ptr : nullptr; }

    void reset() {
        if (ctrl) {
            ctrl->lock.lock();
            ctrl->strong_count--;
            bool should_delete = (ctrl->strong_count == 0);
            ctrl->lock.unlock();
            if (should_delete) {
                delete ctrl->ptr;
                if (ctrl->weak_count == 0) {
                    delete ctrl;
                }
            }
            ctrl = nullptr;
        }
    }
};

template <typename T>
class WeakPtr {
   public:
    WeakPtr(const StrongPtr<T>& src) {
        ctrl = src.ctrl;
        if (ctrl) {
            ctrl->lock.lock();
            ctrl->weak_count++;
            ctrl->lock.unlock();
        }
    }

    ~WeakPtr() { reset(); }

    WeakPtr(const WeakPtr<T>& src) {
        ctrl = src.ctrl;
        if (ctrl) {
            ctrl->lock.lock();
            ctrl->weak_count++;
            ctrl->lock.unlock();
        }
    }

    WeakPtr& operator=(const WeakPtr<T>& rhs) {
        if (this != &rhs) {
            reset();
            ctrl = rhs.ctrl;
            if (ctrl) {
                ctrl->lock.lock();
                ctrl->weak_count++;
                ctrl->lock.unlock();
            }
        }
        return *this;
    }

    StrongPtr<T> promote() const {
        StrongPtr<T> sptr;
        if (ctrl) {
            ctrl->lock.lock();
            if (ctrl->strong_count > 0) {
                ctrl->strong_count++;
                sptr.ctrl = ctrl;
            }
            ctrl->lock.unlock();
        }
        return sptr;
    }

   private:
    impl::shared::ctrlBlock<T>* ctrl;

    void reset() {
        if (ctrl) {
            impl::shared::ctrlBlock<T>* toDelete = nullptr;
            ctrl->lock.lock();
            ctrl->weak_count--;
            if (ctrl->strong_count == 0 && ctrl->weak_count == 0) {
                toDelete = ctrl;
            }
            ctrl->lock.unlock();
            if (toDelete) {
                delete toDelete;
            }
            ctrl = nullptr;
        }
    }
};