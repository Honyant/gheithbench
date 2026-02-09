/*
 * This is a cache synchronization test. 
 *
 * I apologize for the length of the test case; most of it is setup on my end (see below if you are curious). 
 * Hopefully, this message prevents you from actually having to read the code.
 * 
 * Here is what I test:
 * 	(1) You free your caches if you allocate a separate cache for every Ext2 object.
 * 		How: I just create a bunch of redundant Ext2 objects and check if the heap becomes full.
 *	(2) You synchronize your cache accesses.
 *		How: I need a conflict miss to test this, since files that map to different sets in the cache won't interfere with each other.
 *			I search for a pair (A, B) of files whose contents cause a conflict miss, using a filesystem built around a slow IDE.
 *			I then use a filesystem built around a zero-latency IDE to repeatedly and concurrently read those files.
 * 	(3) You don't lock the entire cache.
 * 		How: I read from the same pair (A, B) of conflicting files in part (2), now using the slow IDE.
 * 			Meanwhile, I read from a file C that's already in the cache and doesn't conflict with those files.
 * 			Reads from the cache-resident file should never take as long as going to disk. 			
 *
 * SETUP INFO:
 * The test case is long because of two constraints that force me to detect conflict misses at runtime: 
 * 	(1) I only get ~100 KB for my directory, while y'all get (in principle) up to 5 MB for your cache. Thus, I can't force a capacity miss.
 * 	(2) A priori, I don't know how your cache works. 
 * 		- This is actually still a problem for me, since if you have a set-associative (or fully associative) cache I probably can't force a conflict miss either.
 *		- However, associative caches are likely to fail if they are not properly synchronized anyway.
 *		- There's still a problem! If your cache is larger than 64 KB, it turns out that modulo indexing won't produce any conflicts (probably because the files are laid out in adjacent disk blocks).
 *		- Thus, THIS TEST CASE IS ONLY MEANINGFUL IF YOUR CACHE IS 64 KB OR SMALLER.	
 *
 * For the intellectually curious, here is how I find a conflict miss:
 * >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 *                   <<<<<<<<<<<<<<<<<<<
 *                    >>>>>>>>
 *                   A       B
 * (1) I iterate forward through the files, reading them and populating the cache.
 * (2) I iterate backward through the files. Eventually, I hope to find a file A that takes longer to load than usual. This must have conflicted with some more recent file.
 * (3) I iterate forward through the files again starting at A. Eventually, I will find a file B that takes longer than usual to load, even though it did not before. B must have caused the conflict with A.
 */


#include "debug.h"
#include "kernel.h"
#include "ext2.h"
#include "shared.h"
#include "barrier.h"
#include "pit.h"
#include "libk.h"
#include "threads.h"
#include "heap.h"

constexpr uint32_t BLOCK_SIZE = 1024;
char buf[BLOCK_SIZE];

constexpr uint32_t IDE_LATENCY = 10;
constexpr uint32_t MIN_CACHE_SIZE = 16; // assumes you have at least this many sectors in your cache for free/delete testing
constexpr uint32_t NUM_FILES = 65; // root (".") plus 64 regular files

// please ignore this, it just generates the filenames
class StringOutputStream : public OutputStream<char> {
	char* pos;
public:
	StringOutputStream(char* str) : pos(str) {};
	void put(char v) override {
		*pos++ = v;
	}	
};
char const *const *const filenames = [] {
	char** initial_value = (char**) malloc(NUM_FILES * sizeof(char*));
	initial_value[0] = (char*) malloc(2);
	initial_value[0][0] = '.';
	initial_value[0][1] = 0;
	for (uint32_t i = 1; i < NUM_FILES; i++) {
		initial_value[i] = (char*) malloc(4);
		StringOutputStream s{initial_value[i]};
		K::snprintf(s, 4, "%03d", i);
	}
	return initial_value;
}();
// filenames = {".", "001", "002", "003", ..., "064"}

void free_pass() {
	Debug::printf("| you got a free pass: I couldn't cause a conflict miss\n"); // this goes to the .raw file only
	Debug::printf("*** you synchronize your cache accesses\n");
	Debug::printf("*** you don't lock the entire cache\n");
	Debug::printf("*** done\n");
	Debug::shutdown();
}

void kernelMain() {
	StrongPtr<Ide> ide{new Ide(1,2*IDE_LATENCY)};
	StrongPtr<Ext2> ext2{new Ext2(ide)};
	StrongPtr<Ide> freeloader{new Ide(1,0)};
	StrongPtr<Ext2> fast_ext2{new Ext2(freeloader)};
	
	{ // (1) test cache freeing
		StrongPtr<Ext2> non_singleton{}; // garbage collected
		const uint32_t max_caches_in_heap = (5 << 20) / (512 * MIN_CACHE_SIZE);	
		for (uint32_t i = 0; i < max_caches_in_heap + 1; i++) {
			non_singleton = StrongPtr<Ext2>{new Ext2(freeloader)};		
		}
		if (non_singleton == nullptr) Debug::shutdown(); // otherwise the compiler complains about an "unused variable"	
		Debug::printf("*** you free your caches if you allocate a separate cache for every Ext2 object\n");
	}
	
	StrongPtr<Node>* file_nodes = new StrongPtr<Node>[NUM_FILES];
	{ // gather the nodes and sanity check
		// sanity check (+3 comes from "..", and "lost+found"; "." is already counted in NUM_FILES)
		Debug::printf("*** %d of %d entries counted\n", ext2->root->entry_count(), NUM_FILES + 2); 
		for (uint32_t i = 0; i < NUM_FILES; i++) {
			StrongPtr<Node> file_node = ext2->find(ext2->root, filenames[i]); 	
			if (file_node == nullptr) {
				Debug::printf("*** error: %s not found\n", filenames[i]); // sanity check
				Debug::shutdown();
			}
			file_nodes[i] = file_node;
		}
	}
	
	uint32_t A, B, C;
	{ // find conflicting files A and B and a non-conflicting file C		
		for (uint32_t i = 1; i < NUM_FILES; i++) {
			file_nodes[i]->read_block(0, buf);
		}
		for (uint32_t i = NUM_FILES - 2; ; i--) {
			uint32_t tick = Pit::jiffies;
			file_nodes[i]->read_block(0, buf);
			uint32_t tock = Pit::jiffies;
			if (tock - tick >= IDE_LATENCY) {
				A = i;
				break;
			}
			if (i == 0) free_pass();
		}
		for (uint32_t i = A + 1; ; i++) {
			uint32_t tick = Pit::jiffies;
			file_nodes[i]->read_block(0, buf);	
			uint32_t tock = Pit::jiffies;
			if (tock - tick >= IDE_LATENCY) {
				B = i;
				break;
			}
			if (i == NUM_FILES - 1) free_pass(); // I'm not sure how this would happen but OK
		}
		if (A < NUM_FILES - 2) {
			C = (B == A + 1) ? B + 1 : B - 1;
		}
		else {
			for (uint32_t i = A - 1; ; i--) {
				uint32_t tick = Pit::jiffies;
				file_nodes[i]->read_block(0, buf);	
				uint32_t tock = Pit::jiffies;
				if (tock - tick < IDE_LATENCY) {
					C = i;
					break;
				}
				if (i == 0) {
					Debug::printf("*** error: too many cache misses, empirically you don't cache or your cache only holds one line\n");
					Debug::shutdown();
				}
			}
		}
	}
		
	{ // (2) test concurrency between conflicting reads
		StrongPtr<Node> A_fast_node = fast_ext2->find(fast_ext2->root, filenames[A]);
		StrongPtr<Node> B_fast_node = fast_ext2->find(fast_ext2->root, filenames[B]);	
		char *A_reference = new char[BLOCK_SIZE], *B_reference = new char[BLOCK_SIZE], *A_actual = new char[BLOCK_SIZE], *B_actual = new char[BLOCK_SIZE];
		A_fast_node->read_block(0, A_reference);
		B_fast_node->read_block(0, B_reference);
		StrongPtr<Barrier> bar{new Barrier(3)};

		thread([bar, A_fast_node, A_actual, A_reference] () mutable {
			for (uint32_t i = 0; i < 1000; i++) {
				A_fast_node->read_block(0, A_actual);
				for (uint32_t j = 0; j < 4; j++) {
					if (A_reference[j] != A_actual[j]) {
						Debug::printf("*** error: cache accesses not synchronized\n");	
						Debug::shutdown();
					}
				}
			}
			bar->sync();		
		});
		thread([bar, B_fast_node, B_actual, B_reference] () mutable {
			for (uint32_t i = 0; i < 1000; i++) {
				B_fast_node->read_block(0, B_actual);
				for (uint32_t j = 0; j < 4; j++) {
					if (B_reference[j] != B_actual[j]) {
						Debug::printf("*** error: cache accesses not synchronized\n");
						Debug::shutdown();
					}
				}
			}
			bar->sync();		
		});
		bar->sync();
		Debug::printf("*** you synchronize your cache accesses\n");
		delete[] A_reference, delete[] B_reference, delete[] A_actual, delete[] B_actual;
	}
	
	{ // (3) test that cache lines are locked individually, not globally
		StrongPtr<Node> A_slow_node = file_nodes[A];
		StrongPtr<Node> B_slow_node = file_nodes[B];
		StrongPtr<Node> C_node = file_nodes[C];

		StrongPtr<Barrier> bar{new Barrier(2)};
		thread([bar, C_node] () mutable {
			for (uint32_t i = 0; i < 1000; i++) {
		       		uint32_t tick, tock;
				tick = Pit::jiffies;
				C_node->read_block(0, buf);
				tock = Pit::jiffies;
				if (tock - tick >= IDE_LATENCY) {
					Debug::printf("*** error: unnecessary global locking of cache accesses\n");
					Debug::shutdown();			
				}
			}
			bar->sync();			
		});
		A_slow_node->read_block(0, buf);	
		B_slow_node->read_block(0, buf);
		bar->sync();
		Debug::printf("*** you don't lock the entire cache\n");
	}
	
	delete[] file_nodes;

	{ // this part doesn't really test anything, it just demonstrates a quirk of the IDE code; why does this work?
		StrongPtr<Ide> latency_one_ide{new Ide(1,1)};
		for (uint32_t i = 0; i < 10001; i++) {
			latency_one_ide->read_block(i,buf);	
		}
		Debug::printf("*** done\n");
	}
}
