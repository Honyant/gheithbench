#ifndef WALMART_OMP_H
#define WALMART_OMP_H

#include "sys.hpp"
#include "stdint.h"
#include "libc.hpp"

// Terrible implementations of some of the functionality of OpenMP.
namespace WOMP {
	void set_num_threads(uint32_t num); // noop if num == 0
	uint32_t get_num_threads();
	uint32_t get_thread_num();
	
	// users shouldn't need to call these functions directly
	// but they are called by template functions
	// so unfortunately they must be included in the header file
	void split();
	void join();

	extern int get_critical_id();
       	
	template <typename Work> 
	void parallel(Work work, uint32_t num_threads = 0) {
		uint32_t old_num_threads = get_num_threads();
		set_num_threads(num_threads);
		split();
		work();
		join();
		set_num_threads(old_num_threads);
	}
	
	template <typename NumType, typename Work>
	void parallel_for(Work iteration, NumType start, NumType step, NumType stop, uint32_t block_size = 1, uint32_t num_threads = 0) {
		if (stop < start) return;
		if (!num_threads) num_threads = get_num_threads();
		if (!block_size) block_size = 1;
		NumType block_length = step * block_size;
		NumType block_increment = block_length * num_threads;
		parallel([iteration, start, step, stop, block_size, block_length, block_increment] {
			NumType block_start = start + block_length * get_thread_num();
			for ( ; block_start + block_length < stop; block_start += block_increment) {
				NumType i = block_start;
				for (uint32_t j = 0; j < block_size; j++, i += step) {
					iteration(i);
				}
			}
			for (NumType i = block_start; i < stop; i += step) {
				iteration(i);
			}
		}, num_threads); 	
	}

	// reduce should take a destination (OutType*) and an operand (OutType), updating the destination: += is a reduction
	// reduce should be associative and commutative
	// map can be any function taking a NumType and returning an OutType 
	// identity is the identity of the reduction (0 for addition, 1 for multiplication, etc.)
	// currently this functionality doesn't work
	template <typename NumType, typename OutType, typename Mapping, typename Reduction>
	void parallel_for_reduction(Mapping map, Reduction reduce, OutType identity, OutType* output, NumType start, NumType step, NumType stop, uint32_t block_size = 1, uint32_t num_threads = 0) {
		if (!num_threads) num_threads = get_num_threads();
		OutType* accumulators = (OutType*) naive_mmap(num_threads * sizeof(OutType), true, {}, 0); // false sharing :/
		for (uint32_t i = 0; i < num_threads; i++) {
			accumulators[i] = identity;
		}
		auto iteration = [accumulators, map, reduce] (NumType operand) {
			reduce(&accumulators[get_thread_num()], map(operand));	
		};
		parallel_for(iteration, start, step, stop, block_size, num_threads);
		for (uint32_t i = 0; i < num_threads; i++) {
			reduce(output, accumulators[i]);
		}
		naive_unmap(accumulators); // unfortunately this doesn't do anything on our system	
	}
	
	template <typename Work>
	void critical(Work work, int id) {
		down(id);
		work();
		up(id);	
	}

	template <typename Work>
	void atomic(Work work) {
		critical(work); // I'm not a compiler, so I can't do much better than this
	}

	template <typename Work>
	void master(Work work) {
		if (!get_thread_num()) work();
	}

	template <typename Work>
	void single(Work work) {
		master(work); // I'm lazy
	}
}

#endif
