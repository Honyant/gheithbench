#include "heap/heap.h"
#include "heap/panic.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define HEAP_SIZE (1 << 20)
long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE / sizeof(long)];

int main() {

  long num_to_allocate = 1000;
  long *ptrs[num_to_allocate];

  /* The following test common free merge strategies.
   * If you are failing one, try commenting out the others to isolate the case.
   * You got this!
   * */
  // No merge
  ptrs[0] = malloc(64);
  ptrs[1] = malloc(64);
  ptrs[2] = malloc(64);
  free(ptrs[1]);
  printf("No merge complete\n");
  free(ptrs[0]);
  free(ptrs[2]);

  // Left merge
  ptrs[0] = malloc(64);
  ptrs[1] = malloc(64);
  ptrs[2] = malloc(64);
  free(ptrs[1]);
  free(ptrs[0]);
  printf("Left merge complete\n");
  free(ptrs[2]);

  // Right merge
  ptrs[0] = malloc(64);
  ptrs[1] = malloc(64);
  ptrs[2] = malloc(64);
  free(ptrs[0]);
  free(ptrs[1]);
  printf("Right merge complete\n");
  free(ptrs[2]);

  // Double merge
  ptrs[0] = malloc(64);
  ptrs[1] = malloc(64);
  free(ptrs[0]);
  free(ptrs[1]);
  printf("Double merge complete\n");

  /** This tests for alignment with different block sizes.
   *  Make sure you are aligning all your pointers to 16-bytes
   * */
  for (long i = 1; i < num_to_allocate; i++) {
    ptrs[i] = malloc(i);
    if (ptrs[i] == 0) {
      panic("*** failed to allocate block of size %ld\n", i);
    }
    if ((size_t)(ptrs[i]) % 16 != 0) {
      panic("Block allocated to be %ld was not aligned!\n", i);
    }
  }

  // Try freeing everything
  for (long i = 1; i < num_to_allocate; i++) {
    free(ptrs[i]);
  }
  printf("Successfully allocated and freed aligned block sizes\n");

  /** This tests if the memory endpoints can actually be written to
   *  without causing a SIGSEV or messing up the heap implementation.
   *  If this fails, make sure you are putting the footer in the right place.
   * */
  for (long i = 1; i < 50; i++) {
    ptrs[i] = malloc(i*sizeof(long));
    ptrs[i][0] = 998;
    ptrs[i][i - 1] = 999;
  }
  for (long i = 1; i < 50; i++) {
    free(ptrs[i]);
  }
  printf("Successfully able to read and write from the values!\n");

  /** Test malloc and free edge cases
   *  Most of these are single line changes if you don't pass
   * */
  // malloc(0) should increment mCount
  long current_count = mCount;
  malloc(0);
  if (mCount != current_count + 1)
    panic("malloc(0) should increment mCount!");

  // free(0) should increment mCount
  current_count = fCount;
  free(0);
  if (fCount != current_count + 1)
    panic("free(0) should increment fCount!");

  // malloc(size greater than heap_size) should increment mCount and return null
  current_count = mCount;
  malloc(HEAP_SIZE + 1);
  if (mCount != current_count + 1)
    panic("malloc(size greater than heap_size) should increment mCount and "
          "return null");

  return 0;
}
