#include "debug.h"
#include "smp.h"
#include "debug.h"
#include "config.h"

#include "threads.h"

namespace impl::threads {
    // Limitations of given implementation
    //    - single core only
    //    - no preemption

    struct FakeThread: public TCB {

        void doit() override {
            PANIC("should never get here");
        }

    };

    TCB* init() {
        return new FakeThread();
    }

    TCB* current_thread = init();
    Queue<TCB, NoLock> ready_queue{};

    extern "C" void context_switch(uintptr_t* from, uint32_t to);

    void thread_entry() {
        auto me = current_thread;
        ASSERT(me != nullptr);
        me->doit();
        stop();
    }


}

void yield() {
    using namespace impl::threads;

    block(&ready_queue);   
}

void stop() {
    using namespace impl::threads;
    while(true) block(&ready_queue); // why is this a bad idea?
}


