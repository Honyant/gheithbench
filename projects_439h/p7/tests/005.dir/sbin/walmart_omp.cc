#include "walmart_omp.h"

namespace WOMP {
	static constexpr uint32_t _NUM_CORES = 4; // don't have a syscall for this so have to hardcode
	static uint32_t _NUM_THREADS = _NUM_CORES;
	static uint32_t _ct = 0;
	static int* _children = (int*) malloc((_NUM_THREADS - 1) * sizeof(int));
	static bool _parallel_flag = 0; // prevents reentrancy in parallel sections

	void set_num_threads(uint32_t num) {
		if (_parallel_flag) exit(1);
		if (!num) return;
		_NUM_THREADS = num;
		free(_children);
		_children = (int*) malloc((_NUM_THREADS - 1) * sizeof(int));
	}
	uint32_t get_num_threads() {
		return _NUM_THREADS;
	}
	uint32_t get_thread_num() {
		return _ct;
	}
	void split() {
		if (_parallel_flag) exit(1);
		_parallel_flag = 1;
		_ct = _NUM_THREADS - 1;
		int cd; 
		while (_ct && (cd = fork())) {
			_children[--_ct] = cd;
		}
	}
	void join() {
		if (_ct) exit(0);
		for (uint32_t c = 0; c < _NUM_THREADS - 1; c++) {
			uint32_t status;
			wait(_children[c], &status);
			if (status) exit(status);
		}
		_parallel_flag = 0;
	}
	
	int get_critical_id() {
		return sem(1); // this should probably handle errors
	}
}
