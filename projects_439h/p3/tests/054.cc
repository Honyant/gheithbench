#include "atomic.h"
#include "debug.h"
#include "loop.h"
#include "pit.h"
#include "threads.h"

// can't get much bigger without overflowing jiffies after converting to ms
#define FOREVER 4'000'000

// Gheith's Barrier from p1
class SpinBarrier {
	Atomic<uint32_t> counter;
public:
	SpinBarrier(uint32_t counter) : counter(counter) {
	}

	SpinBarrier(const SpinBarrier &) = delete;

	void sync() {
		counter.add_fetch(-1);
		while (counter.get() != 0) {
			iAmStuckInALoop(false);
		}
	}
};

class TimeMachine {
	SpinBarrier starting;
	SpinBarrier stopping;
public:
	TimeMachine() : starting(kConfig.totalProcs), stopping(kConfig.totalProcs) {
	}

	void travelTo(uint32_t seconds) {
		for (uint32_t i = 0; i < kConfig.totalProcs; i++) {
			// Occupy all cores in order to safely modify jiffies
			thread([this, seconds] {
				Interrupts::protect([this, seconds] {
					this->starting.sync();
					if (SMP::me() == 0) {
						// Time travel!
						Pit::jiffies = Pit::secondsToJiffies(seconds);
					}
					this->stopping.sync();
				});
				stop();
			});
		}
	}
};

void kernelMain() {
	TimeMachine timeMachine;
	// Schedule a thread to sleep for a long time
	thread([] {
		sleep(FOREVER);
		uint32_t wakeupTime = Pit::jiffies;
		Debug::printf("*** Awake\n");

		if (wakeupTime < Pit::secondsToJiffies(FOREVER)) {
			Debug::printf("*** Woke up too soon! (Time is %d, should be at least %d)\n",
			      wakeupTime, Pit::secondsToJiffies(FOREVER));
		}
		Debug::shutdown();
	});

	timeMachine.travelTo(FOREVER);

	// Do not return or else we could call shutdown prematurely
	stop();
}
