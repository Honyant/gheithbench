#include "threads.h"
#include "debug.h"
#include "barrier.h"
#include "bb.h"
#include "blocking_lock.h"

#define FOREVER 1000000000

BB<int>* test_buffer = new BB<int>(2);

Barrier* sleep_barrier = new Barrier(MAX_PROCS + 1);
Barrier* lock_barrier = new Barrier(MAX_PROCS + 1);
Barrier* interrupt_barrier = new Barrier(MAX_PROCS + 1);

BlockingLock* barrier_lock = new BlockingLock();

BB<bool>* sleep_buffer = new BB<bool>(MAX_PROCS); 
BB<bool>* lock_buffer = new BB<bool>(MAX_PROCS);
BB<bool>* interrupt_buffer = new BB<bool>(MAX_PROCS);

BlockingLock* buffer_lock = new BlockingLock();

// need lvalues to pass to bounded buffers
int ints[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
bool bools[2] = {false, true};

void kernelMain(void) {
	Debug::printf("*** Test started! You can print things now.\n");

	/* (NON-CONCURRENT) BOUNDED BUFFER TESTS */

	// no concurrency or blocking yet
	test_buffer->put(ints[0]);
	test_buffer->put(ints[1]);
	if (test_buffer->get() != 0) {
		Debug::printf("*** ERROR! Corrupted value in bounded buffer.\n");
	}
	if (test_buffer->get() != 1) {
		Debug::printf("*** ERROR! Corrupted value in bounded buffer.\n");
	}
	Debug::printf("*** Non-concurrent, non-blocking bounded buffer operations finished.\n");

	// add blocking
	test_buffer->put(ints[2]);
	test_buffer->put(ints[3]);
	thread([] {
		Debug::printf("*** Buffer at capacity, taking one off.\n");
		if (test_buffer->get() != 2) {
			Debug::printf("*** ERROR! Corrupted value in bounded buffer.\n");	
		}
	});
	test_buffer->put(ints[4]);
	Debug::printf("*** Can produce now.\n");
	if (test_buffer->get() != 3) {
		Debug::printf("*** ERROR! Corrupted value in bounded buffer.\n");
	}
	if (test_buffer->get() != 4) {
		Debug::printf("*** ERROR! Corrupted value in bounded buffer.\n");
	}
	thread([] {
		Debug::printf("*** Buffer empty, putting one on.\n");
		test_buffer->put(ints[5]);	
	});
	if (test_buffer->get() != 5) {
		Debug::printf("*** ERROR! Corrupted value in bounded buffer.\n");
	}
	Debug::printf("*** Can consume now.\n");
	Debug::printf("*** Non-concurrent, blocking bounded buffer operations finished.\n");

	/* SLEEP TESTS */
	
	thread([] {
		// Debug::printf("*** Main thread went to sleep.\n"); // this is probably unsafe
	});
	sleep(1);
	Debug::printf("*** Main thread woke up.\n");

	// check that main thread can run while other threads sleep
	for (int n = 0; n < MAX_PROCS; n++) {
		thread([] {
			sleep_barrier->sync(); // force these threads to run before main
			sleep(FOREVER);
			Debug::printf("*** ERROR! Thread woke up from sleep prematurely.\n");
		});
	}
	sleep_barrier->sync();
	Debug::printf("*** Sleeping threads blocked.\n");

	/* BLOCKING LOCK TEST */

	// same thing but with the blocking lock
	barrier_lock->lock();
	for (int n = 0; n < MAX_PROCS; n++) {
		thread([] {
			lock_barrier->sync();
			barrier_lock->lock(); // oops, we double-locked it...		
		});
	}
	lock_barrier->sync();
	Debug::printf("*** Blocking lock blocked.\n");

	/*  PIT TEST */

	// again, now with the PIT
	for (int n = 0; n < MAX_PROCS; n++) {
		thread([] {
			interrupt_barrier->sync();
			while(true);
		});
	}
	interrupt_barrier->sync();
	Debug::printf("*** Interrupts reenabled.\n");

	/*
	 * BOUNDED BUFFER CONCURRENCY TESTS
	 */

	// run the three previous tests again using bounded buffers instead of barriers
	for (int n = 0; n < MAX_PROCS; n++) {
		thread([] {
			sleep_buffer->put(bools[true]);
			sleep(FOREVER);
			Debug::printf("*** ERROR! Thread woke up from sleep prematurely.\n");			
		});
	}
	for (int n = 0; n < MAX_PROCS; n++) {
		thread([] {
			if (!sleep_buffer->get()) {
				Debug::printf("*** ERROR! Corrupted value in bounded buffer.\n");
			}		
		});
	}
	Debug::printf("*** Sleeping threads blocked (bounded buffer).\n");

	buffer_lock->lock();
	for (int n = 0; n < MAX_PROCS; n++) {
		thread([] {
			lock_buffer->put(bools[true]);
			buffer_lock->lock();			
		});
	}
	for (int n = 0; n < MAX_PROCS; n++) {
		thread([] {
			if (!lock_buffer->get()) {
				Debug::printf("*** ERROR! Corrupted value in bounded buffer.\n");
			}		
		});
	}
	Debug::printf("*** Blocking lock blocked (bounded buffer).\n");
	
	for (int n = 0; n < MAX_PROCS; n++) {
		thread([] {
			interrupt_buffer->put(bools[true]);
			while(true);
		});
	}
	for (int n = 0; n < MAX_PROCS; n++) {
		thread([] {
			if (!interrupt_buffer->get()) {
				Debug::printf("*** ERROR! Corrupted value in bounded buffer.\n");
			}		
		});
	}
	Debug::printf("*** Interrupts reenabled (bounded buffer).\n");

	/* END */
	Debug::printf("*** All tests finished!\n");
}
