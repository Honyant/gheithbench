#include "threads.h"
#include "debug.h"
#include "bb.h"
#include "blocking_lock.h"
#include "barrier.h"

#define MAX_READERS 3
#define BUFFER_CAPACITY MAX_READERS
/*
Use Bounded Buffer as a synchronization primitive to implement a read-write lock.
Assumes Blocking Lock and BB are implemented.
*/

BB<int>* rw_lock_buffer = new BB<int>(BUFFER_CAPACITY); // Bounded buffer controlling access for readers
BlockingLock* write_lock = new BlockingLock();          // Lock for managing exclusive writes
int shared_resource = 0; // Shared resource for readers and writers
int LOCK_CONST = 1; //We don't actually care about the value.

void read_lock() {
    rw_lock_buffer->get(); // Readers get access by consuming a token from the buffer
}

void read_unlock() {
    rw_lock_buffer->put(LOCK_CONST); // Readers release access by putting a token back
}

void writer_lock() {
    write_lock->lock();     // Ensure only one writer can write at a time
    for (int i = 0; i < BUFFER_CAPACITY; i++) {
        rw_lock_buffer->get(); // Writer consumes all tokens, blocking readers
    }
}

void writer_unlock() {
    for (int i = 0; i < BUFFER_CAPACITY; i++) {
        rw_lock_buffer->put(LOCK_CONST); // Writer releases all tokens, allowing readers
    }
    write_lock->unlock();       // Release the writer lock for other writers
}

void reader_thread(int id) {
    for (int i = 0; i < 2; i++) { // Each reader reads twice
        bool was = Interrupts::disable();
        read_lock();
        Debug::printf("Reader %d reads value: %d\n", id, shared_resource);
        read_unlock();
        Interrupts::restore(was);
        sleep(1);
    }
}

void writer_thread(int id) {
    for (int i = 0; i < 2; i++) { // Each writer writes twice
        bool was = Interrupts::disable();
        writer_lock();
        shared_resource++;
        Debug::printf("Writer %d writes value: %d\n", id, shared_resource);
        writer_unlock();
        Interrupts::restore(was);
        sleep(1);
    }
}

Barrier* b1 = new Barrier(MAX_READERS + 3);

void kernelMain(void) {
    // Initialize bounded buffer with "tokens" for readers
    for (int i = 0; i < BUFFER_CAPACITY; i++) {
        rw_lock_buffer->put(LOCK_CONST); // Initialize buffer with available tokens for readers
    }

    // Create reader threads
    for (int i = 0; i < MAX_READERS; i++) {
        thread([i] { reader_thread(i); b1->sync();});
    }

    // Create two writer threads to simulate concurrent writing attempts
    thread([] { writer_thread(0); b1->sync();});
    thread([] { writer_thread(1); b1->sync();});

    b1->sync();
    Debug::printf("*** done\n");
}
