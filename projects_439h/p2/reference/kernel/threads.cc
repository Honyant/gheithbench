#include "debug.h"
#include "smp.h"
#include "debug.h"
#include "config.h"
#include "heap.h"
#include "queue.h"

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

    TCB** init_threads() {
        TCB** threads = (TCB**) malloc(kConfig.totalProcs*sizeof(TCB*));
        for(uint32_t i=0;i<kConfig.totalProcs;i++) threads[i]= new FakeThread();
        return threads;
    }

    Queue<TCB, SpinLock>** init_queues(){
        Queue<TCB, SpinLock>** queues = (Queue<TCB, SpinLock>**) malloc(kConfig.totalProcs*sizeof(Queue<TCB, SpinLock>*));
        return queues;
    }

    TCB** current_threads = init_threads();

    TCB** prev_threads = init_threads();
    
    Queue<TCB, SpinLock> ready_queue{};

    extern "C" void context_switch(uintptr_t* from, uint32_t to);

    void thread_entry() {
        auto& g = prev_threads[SMP::me()];
        if(g->stop){
            delete g;
        }else{
            if(g->isready!=nullptr){
                if(!*g->isready){
                    g->lock->lock();
                        if(!*g->isready) g->prev_queue->add(g);
                        else ready_queue.add(g);
                    g->lock->unlock();
                }else ready_queue.add(g);
                g->isready = nullptr;
            }else if(g->barrier_sephamore!=nullptr){
                if(g->barrier_sephamore->get()>0){
                    g->lock->lock();
                        if(g->barrier_sephamore->get()>0)  g->prev_queue->add(g);
                        else ready_queue.add(g);
                    g->lock->unlock();
                }else ready_queue.add(g); 
                g->barrier_sephamore = nullptr;
            }else{
                g->prev_queue->add(g);
            }
        }
        auto me = current_threads[SMP::me()];
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
    // need to delete tcb after context switch
    current_threads[SMP::me()]->stop=true;
    while(true) block(&ready_queue); // why is this a bad idea?
}


