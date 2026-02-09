#include "threads.h"

#include "config.h"
#include "debug.h"
#include "pit.h"
#include "smp.h"

namespace impl::threads {



struct FakeThread : public TCB {
    void doit() override { PANIC("should never get here"); }
};

TCB** init_current_threads() {
    auto ct = (TCB**)malloc(kConfig.totalProcs * sizeof(TCB*));
    for (uint32_t i = 0; i < kConfig.totalProcs; ++i)
        ct[i] = new FakeThread();
    return ct;
}

TCB** init_helper_threads() {
    auto ct = (TCB**)malloc(kConfig.totalProcs * sizeof(TCB*));
    for (uint32_t i = 0; i < kConfig.totalProcs; ++i){
        ct[i] = new Helper();
    }
    return ct;
}

TCB** helper_threads = init_helper_threads();
TCB** current_threads = init_current_threads();
Queue<TCB, SpinLock> ready_queue{};
Queue<TCB, SpinLock> death_queue{};
SpinLock death_lock{};

Atomic<bool> reaper_started = false;

SpinLock sleep_lock;
TCB* sleep_list = nullptr;

void reaper_function() {
    while (true) {
        Debug::printf("Reaper: checking for dead threads\n");
        death_lock.lock();
        auto boutta_die = death_queue.remove();
        if (boutta_die != nullptr) {
            Debug::printf("Reaper: deleting thread\n");
            delete boutta_die;
        }
        death_lock.unlock();
        yield();
    }
}

ReaperThread* reaper_thread = nullptr;

TCB* me_thread() {
    bool was = Interrupts::disable();
    TCB* me = current_threads[SMP::me()];
    Interrupts::restore(was);
    return me;
}

void post_switch(){
    auto me = me_thread();
    auto* g = me->came_from;
    g->busy = g->inApit || g->is_helper;
    if (g->is_helper || g->is_sleeping || g->inApit) {
    } else if (g->stop) {
        death_queue.add(g);
        // delete g;
    } else if (g->nl_queue != nullptr) {
        g->lock->lock();
        if ((--(*g->dc_semaphore_count)) < 0) {
            g->nl_queue->add(g);
        } else {
            ready_queue.add(g);
        }
        g->lock->unlock();
        g->clearsem();
    } else if (g->spin_queue != nullptr) {
        g->spin_queue->add(g);
    } else {
        ready_queue.add(g);
    }
    me->busy = me->inApit || me->is_helper;
}
void offline_interrupt_handler(){
    while(true){
        auto was = Interrupts::disable();
        TCB* me = helper_threads[SMP::me()];
        Interrupts::restore(was);
         uint32_t current_jiffies = Pit::jiffies;
            if (sleep_list != nullptr && sleep_list->wake_jiffies <= current_jiffies){
                sleep_lock.lock();
                current_jiffies = Pit::jiffies;
                if (sleep_list != nullptr && sleep_list->wake_jiffies <= current_jiffies) {
                    TCB* to_wake = sleep_list;
                    sleep_list = sleep_list->next_sleep;
                    to_wake->next_sleep = nullptr;
                    to_wake->is_sleeping = false;
                    to_wake->busy = true;
                    Debug::printf("waking up in wake_sleeping_threads\n");
                    continue;
                }
                sleep_lock.unlock();
            }
        TCB* next = ready_queue.remove();
        if (next == nullptr) {
            next = me->came_from;
            next->busy = true;
            current_threads[SMP::me()] = next;
            next->came_from = me;
            context_switch(&me->sp, &next->sp);
            post_switch();
            continue;
        }
        // Debug::printf("thread %d\n", SMP::me());
        // if(next->is_sleeping){
        //     Debug::printf("wake up gettgsdf");
        // }
        ready_queue.add(me->came_from);
        next->busy = true;
        current_threads[SMP::me()] = next;
        next->came_from = me;
        context_switch(&me->sp, &next->sp);
        post_switch();
    }
}

void thread_entry() {
    post_switch();
    TCB* me = me_thread();
    // me->busy = me->inApit;
    ASSERT(me != nullptr);
    me->doit();
    stop();
}

template <typename LockType>
extern void block(Queue<TCB, LockType>* q) {
    auto was = Interrupts::disable();
    TCB* me = current_threads[SMP::me()];
    Interrupts::restore(was);
    auto next = ready_queue.remove();
    if (next == nullptr) {
        if(!me->inApit) me->busy = false;
        return;
    }

    me->spin_queue = q;

    next->busy = true;
    current_threads[SMP::me()] = next;
    next->came_from = me;
    context_switch(&me->sp, &next->sp);
    post_switch();
}

extern bool blocknl(Queue<TCB, NoLock>* q, int* counter, SpinLock* s) {
    auto was = Interrupts::disable();
    TCB* me = current_threads[SMP::me()];
    Interrupts::restore(was);

    auto next = ready_queue.remove();
    //print current thread pointer address
    if (next == nullptr) {
        if(!me->inApit) me->busy = false;
        return false;
    }
    me->nl_queue = q;
    me->dc_semaphore_count = counter;
    me->lock = s;
    // me->was = Interrupts::isDisabled();
    next->busy = true;
    current_threads[SMP::me()] = next;
    next->came_from = me;
    context_switch(&me->sp, &next->sp);
    post_switch();
    return true;
}


bool block_sleep() {
    auto was = Interrupts::disable();
    TCB* me = current_threads[SMP::me()];
    Interrupts::restore(was);

    auto next = ready_queue.remove();
    //print current thread pointer address
    if (next == nullptr) {
        if(!me->inApit) me->busy = false;
    }
    return false;

    // me->was = Interrupts::isDisabled();
    next->busy = true;
    current_threads[SMP::me()] = next;
    next->came_from = me;
    context_switch(&me->sp, &next->sp);
    post_switch();
    return true;
}

}  // namespace impl::threads

void yield() {
    using namespace impl::threads;
    TCB* me = me_thread();
    me->busy = true;
    block(&ready_queue);
    me = me_thread();
    me->busy = me->inApit || me->is_helper;
}

void sleep(uint32_t sec) {
    uint32_t wake_jiffies = Pit::jiffies + Pit::secondsToJiffies(sec);
    while(Pit::jiffies < wake_jiffies)yield();
    return; using namespace impl::threads;
    auto was = Interrupts::disable();
    TCB* me = me_thread();
    Interrupts::restore(was);
    me->wake_jiffies = wake_jiffies;

    sleep_lock.lock();
    if (sleep_list == nullptr || wake_jiffies < sleep_list->wake_jiffies) {
        me->next_sleep = sleep_list;
        sleep_list = me;
        
    } else {
        TCB* curr = sleep_list;
        while (curr->next_sleep != nullptr && curr->next_sleep->wake_jiffies <= wake_jiffies) {
            curr = curr->next_sleep;
        }
        me->next_sleep = curr->next_sleep;
        curr->next_sleep = me;
    }
    sleep_lock.unlock();

    
    me->busy = true;
    me->is_sleeping = true;

    while(!block_sleep());

    Debug::printf("woke up\n");
}

// [[noreturn]]
void stop() {
    Debug::printf("stopping\n");
    using namespace impl::threads;
    auto me = me_thread();
    // if(!reaper_started.exchange(true)) {
    //     ready_queue.add(reaper_thread);
    //     ready_queue.add(reaper_thread);
    //     ready_queue.add(reaper_thread);
    //     ready_queue.add(reaper_thread);
    //     Debug::printf("reaper started\n");
    // }

    me->stop = true;
    while (true) {
        yield();
    }
}