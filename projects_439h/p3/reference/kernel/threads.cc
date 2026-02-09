#include "threads.h"

#include "config.h"
#include "debug.h"
#include "heap.h"
#include "queue.h"
#include "smp.h"
#include "pit.h"

namespace impl::threads {
// Limitations of given implementation
//    - single core only
//    - no preemption

struct FakeThread : public TCB {
    void doit() override { PANIC("should never get here"); }
};

struct Helper : public TCB {
    void doit() override { helper_doit(); }
};

TCB** init_threads() {
    TCB** threads = (TCB**)malloc(kConfig.totalProcs * sizeof(TCB*));
    for (uint32_t i = 0; i < kConfig.totalProcs; i++)
        threads[i] = new FakeThread();
    return threads;
}
TCB** init_helpers() {
    TCB** helpers = (TCB**)malloc(kConfig.totalProcs * sizeof(TCB*));
    for (uint32_t i = 0; i < kConfig.totalProcs; i++)
        helpers[i] = new Helper();
    return helpers;
}

Queue<TCB, SpinLock>** init_queues() {
    Queue<TCB, SpinLock>** queues = (Queue<TCB, SpinLock>**)malloc(
        kConfig.totalProcs * sizeof(Queue<TCB, SpinLock>*));
    return queues;
}

TCB* me_thread() {
    bool was = Interrupts::disable();
    TCB* me = current_threads[SMP::me()];
    Interrupts::restore(was);
    return me;
}

TCB** helper_threads = init_helpers();

TCB** current_threads = init_threads();

extern "C" void add_and_unlock() {//this is called inside context_siwtch
    auto prev = current_threads[SMP::me()]->came_from;
    if(prev->bblock){
        prev->prev_queue->add(prev);
        prev->bblock->unlock();
    }
}

TCB* sleep_threads = nullptr;
SpinLock* sleep_lock = new SpinLock();

Queue<TCB, SpinLock> ready_queue{};

Queue<TCB, SpinLock> useless{};

extern "C" void unlock(SpinLock* lock) {
    me_thread()->lock->unlock();
}


extern "C" void context_switch(uintptr_t* from, uintptr_t* to);

extern "C" void context_switchl(uintptr_t* from, uintptr_t* to);

void helper_doit(){
    auto was = Interrupts::disable();
    TCB* helper_tcb = current_threads[SMP::me()];
    Interrupts::restore(was);
    post_switch();
    context_switch(&helper_tcb->sp, &helper_tcb->next->sp);
}

void post_switch() {
    auto me = me_thread();
    Interrupts::restore(0);
    auto& g = me->came_from; //g = prev
    if(!g->isApit) g->busy = false;
    if (g->stop) {
        delete g;
    } else {
        if(g->bblock){
            g->bblock = nullptr;
        }
        else if (g->isready != nullptr) {
            g->lock->lock();
            if (g->isready == nullptr) {
                g->prev_queue->add(g);
            } else if (!*g->isready) {
                g->prev_queue->add(g);
            } else {
                ready_queue.add(g);
            }
            g->isready = nullptr;
            g->lock->unlock();
        } else if (g->barrier_semaphore != nullptr) {
            g->lock->lock();
            if (g->barrier_semaphore) {
                g->prev_queue->add(g);
            } else {
                ready_queue.add(g);
            }
            g->lock->unlock();
            g->barrier_semaphore = nullptr;
        } else {
            g->prev_queue->add(g);
        }
    }
    if(!me->isApit) me->busy = false;
}

void thread_entry() {
    post_switch();
    auto me = me_thread();
    ASSERT(me != nullptr);
    me->doit();
    stop();
}

void idle() {
    while (true) {
        yield();
    }
}

}  // namespace impl::threads

void yield() {
    using namespace impl::threads;
    auto me = me_thread();
    me->busy = true;
    block_helper(&ready_queue);
    me = me_thread();
    if(!me->isApit){
        me->busy = false;
    }
}
void stop() {
    using namespace impl::threads;
    // need to delete tcb after context switch
    me_thread()->stop = true;
    while (true) {
    impl::threads::me_thread()->busy = true;
        // ASSERT(impl::threads::me_thread()->busy == true);

        block_helper(&ready_queue);  // why is this a bad idea?
    }
}

void sleep(uint32_t seconds){
    using namespace impl::threads;
    if(seconds == 0){
        return;
    }
    auto endJiffies = Pit::jiffies + Pit::secondsToJiffies(seconds);
    // auto me = me_thread();
    // me->wakeJiffies = endJiffies;
    // sleep_lock->lock();
    // if (sleep_threads == nullptr || sleep_threads->wakeJiffies > endJiffies) {
    //     me->next = sleep_threads;
    //     sleep_threads = me;
    // } else{
    //     TCB* prev = sleep_threads;
    //     TCB* it = sleep_threads->next;
    //     while (it != nullptr && it->wakeJiffies <= endJiffies) {
    //         prev = it;
    //         it = it->next;
    //     }
    //     // Insert after prev
    //     me->next = it;
    //     prev->next = me;
    // }
    while(Pit::jiffies < endJiffies) yield();
    
}