#pragma once

#include "libc.h"

typedef struct {
    int sem_id;
    uint32_t value;
} Promise;

// Initializes the Promise
void Promise_init(Promise *promise) {
    promise->sem_id = sem(0);
    promise->value = 0;
}

// Sets the promise's value and signals the semaphore
void Promise_set(Promise *promise, uint32_t v) {
    promise->value = v;
    up(promise->sem_id);
}

// Waits for the promise's value to be available and then retrieves it
uint32_t Promise_get(Promise *promise) {
    down(promise->sem_id); // get the promise lock
    printf("*** promise value for child: %ld\n", promise->value);
    up(promise->sem_id); // release promise lock

    return promise->value;
}
