#include "go.h"
#include <stdlib.h>
#include <stdio.h>
#include <alloca.h>

struct Routine;
typedef struct Routine Routine;

#define STACK_ENTRIES (8192 / sizeof(uint64_t))

typedef struct Queue {
    struct Routine* head;
    struct Routine* tail;
} Queue;


struct Channel {
    Queue q;
    Value v;
    bool closed;
    bool send; //0 for false, 1 for true
};

struct Routine {
    uint64_t saved_rsp;
    // Each c-routine needs its private stack
    uint64_t stack[STACK_ENTRIES];
    Routine* next;
    Channel* ch;
    Value val;
    Func func;
    bool isMain;
};

/////////

static void addQ(Queue* q, Routine* r) {
    r->next = 0;
    if (q->tail != 0) {
        q->tail->next = r;
    }
    q->tail = r;

    if (q->head == 0) {
        q->head = r;
    }
}

static Routine* removeQ(Queue* q) {
    Routine* r = q->head;
    if (r != 0) {
        q->head = r->next;
        if (q->tail == r) {
            q->tail = 0;
        }
    }
    return r;
}

///////////////////////////////////////////////////

static Routine *current_ = 0;
static Queue ready = {0, 0};
Routine* toBeFreed = 0;
uint64_t* toBeFreed_sp = 0;
extern int main(int argc, char **argv, char **envp);
uint64_t origAlloca;
int _argc;
char** _argv;
char** _envp;

Routine** current() {
    if (current_ == 0) {
        current_ = (Routine*) calloc(sizeof(Routine),1);
    }
    return &current_;
}

/* OSX prepends _ in front of external symbols */
Routine** _current() {
    return current();
}

void _yield(){
    if (!ready.head) exit(0); // we goneeeeee
    Routine* r = removeQ(&ready);
    Routine* old = *current();
    current_ = r;
    _context_switch((uint64_t*)old ,r->saved_rsp);

}

void __wrapper() {

    if (toBeFreed && toBeFreed_sp != (*current())->stack && !(*current())->isMain) {
        free(toBeFreed);
        toBeFreed = 0;
    }
    Routine* curr = *current();
    Value x;
    if (curr->isMain) {
        exit(main(_argc, _argv, _envp));
    }
    else {
        x = curr->func();
    }    
    toBeFreed = curr;
    toBeFreed_sp = curr->stack;
    curr->ch->v = x;
    curr->ch->closed = true;
    while (1) send(curr->ch,x);
}

Channel* go(Func func) { 
    Routine* r = (Routine*) malloc(sizeof(Routine));
    r->stack[STACK_ENTRIES-1] = (uint64_t) &__wrapper; //top byte of the stack should point to __wrapper
    r->saved_rsp = (uint64_t) &r->stack[STACK_ENTRIES - 7]; //should be 7 below
    r->func = func;
    r->ch = channel();
    addQ(&ready,r);
    return r->ch;
}


Channel* me() {
    return (*current())->ch;
}

void again() {
    Routine* r = *current();
    if (r->isMain){
        r->saved_rsp = origAlloca;
        *(uint64_t*) (r->saved_rsp) = (uint64_t) &__wrapper;
        exit(main(_argc, _argv, _envp));
    } else {
        r->stack[STACK_ENTRIES-1] = (uint64_t) &__wrapper; //top byte of the stack should point to __wrapper
        r->saved_rsp = (uint64_t) &r->stack[STACK_ENTRIES - 1];
        _call_func((void*) r->saved_rsp);
    }
}

Channel* channel() {
    Channel* ch = (Channel*) malloc(sizeof(Channel));
    ch->q = (Queue) {0,0};
    return ch;
}

Value receive(Channel* ch) {
    if (!ch->q.head) {
        ch->send = false;
    } if (ch->send) {
        Routine* r = removeQ(&ch->q);
        addQ(&ready,r);
        return r->val;
    } else {
        //adds current to the queue for the channel and yields
        if (ch->closed) return ch->v;
        else{
            addQ(&ch->q,*current());
            _yield();
            return (*current())->val;
        }
    }
}

void __attribute__((constructor)) ____init(int argc, char **argv, char **envp) {
    _argc = argc;
    _argv = argv;
    _envp = envp;
    origAlloca = (uint64_t) alloca(1);
    Routine* mainRoutine = (Routine*) malloc(sizeof(Routine));
    mainRoutine->saved_rsp = (uint64_t) alloca(1);
    origAlloca = mainRoutine->saved_rsp;
    *(uint64_t*) (mainRoutine->saved_rsp) = (uint64_t) &__wrapper;
    mainRoutine->func = (Func) &main;
    mainRoutine->ch = channel();
    mainRoutine->isMain = true;
    current_ = mainRoutine;
    _call_func((void*) mainRoutine->saved_rsp);
}

void send(Channel* ch, Value v) {
    if (!ch->q.head) {
        ch->send = true;
    } if (ch->send) {
        (*current())->val = v;
        addQ(&ch->q,*current());
        _yield();
    } else {
        ch->q.head->val = v;
        addQ(&ready,removeQ(&ch->q));
    }    
}
