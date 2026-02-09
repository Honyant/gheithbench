/* Multi-threaded matrix-vector multiplication.
Each thread works on a single row of the matrix at a time.

This specifically tests barriers.
An incorrect barrier implementation will allow one processor to begin printing before some arithmetic operations are complete.
We spawn the row-computing threads bottom-up because we print top-down: this increases the chance that the first values to print will not be finalized by the time they are printed.
Also, if you spin instead of blocking on a barrier then you will deadlock, because the barrier requires more synchronizing processes than the number of cores.
*/

#include "debug.h"
#include "config.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"

#define ROWS 16
#define COLS 16

int* matrix = new int[ROWS * COLS]{
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};	
int* vector = new int[COLS]{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
int product[ROWS]; // allocate storage space in advance

Barrier* done = new Barrier(ROWS + 1);

void kernelMain(void) {
	for (int i = ROWS - 1; i >= 0; i--) {
		thread([i] {
			// do some work
			int* row_i = matrix + i * ROWS;
			for (int j = 0; j < COLS; j++) {
				product[i] += row_i[j] * vector[j];
			}
			// I'm done
			done->sync();	
		});
	}
	// wait for all my children to finish
	done->sync();
	// print the vector in JSON array format
	Debug::printf("*** [");
	for (int i = 0; i < ROWS - 1; i++) {
		Debug::printf("%d, ", product[i]);
	}
	Debug::printf("%d", product[ROWS-1]);
	Debug::printf("]\n");	
}
