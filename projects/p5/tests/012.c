#include "heap/heap.h"
#include "heap/panic.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define N 40000

#define HEAP_SIZE (1 << 21)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE / sizeof(long)];

void *ptrs[N];

bool check_alignment(void *ptr) { return (long)ptr % 16 == 0; }

int main() {
  // this test will create lots of holes in your heap
  // the purpose is to test your merging and searching
  // algorithms
  //
  // each subtask is separate, should you free properly
  //
  // DISCLAIMER: you should be able to pass this case
  //             with 32 bytes of metadata and O(1)
  //             extra space
  //
  int test_counter = 0;

  // #ifndef TEST_ONE
  // #define TEST_ONE
  //
  // free in order
  test_counter++;

  // populate ptrs
  for (int i = 0; i < N; i++) {
    ptrs[i] = malloc(1);
    if (!check_alignment(ptrs[i])) {
      panic("*** alignment failure at offset %d\n", i);
    }
  }

  // free each set of 8 in order
  for (int i = 0; i < 8; i++) {
    for (int j = i; j < N; j += 8) {
      free(ptrs[j]);
      ptrs[j] = 0;
    }
  }

  // make large alloc to ensure merging
  ptrs[0] = malloc(1 << 20);
  if (!ptrs[0]) {
    panic("*** unable to malloc large block after test %d\n", test_counter);
  } else {
    free(ptrs[0]);
    ptrs[0] = 0;
  }

  // print results
  if (mCount == fCount) {
    printf("*** test %d counts match\n", test_counter);
  } else {
    printf("*** test %d counts do not match (mallocs: %ld, frees: %ld)\n",
           test_counter, mCount, fCount);
  }
  printf("*** test %d finished\n", test_counter);
  // #endif

  // #ifndef TEST_TWO
  // #define TEST_TWO
  //
  // free in reverse order
  test_counter++;

  // populate ptrs
  for (int i = 0; i < N; i++) {
    ptrs[i] = malloc(1);
    if (!check_alignment(ptrs[i])) {
      panic("*** alignment failure at offset %d\n", i);
    }
  }

  // free each set of 8 in reverse order
  for (int i = 7; i >= 0; i--) {
    for (int j = i; j < N; j += 8) {
      free(ptrs[j]);
      ptrs[j] = 0;
    }
  }

  // make large alloc to ensure merging
  ptrs[0] = malloc(1 << 20);
  if (!ptrs[0]) {
    panic("*** unable to malloc large block after test %d\n", test_counter);
  } else {
    free(ptrs[0]);
    ptrs[0] = 0;
  }

  // print results
  if (mCount == fCount + 1) {
    printf("*** test %d counts match\n", test_counter);
  } else {
    printf("*** test %d counts do not match (mallocs: %ld, frees: %ld)\n",
           test_counter, mCount, fCount);
  }
  printf("*** test %d finished\n", test_counter);
  // #endif

  // #ifndef TEST_THREE
  // #define TEST_THREE
  //
  // free in random order
  test_counter++;

  // populate ptrs
  for (int i = 0; i < N; i++) {
    ptrs[i] = malloc(1);
    if (!check_alignment(ptrs[i])) {
      panic("*** alignment failure at offset %d\n", i);
    }
  }

  // free set of eight out of order
  for (int j = 2; j < N; j += 8) {
    free(ptrs[j]);
    ptrs[j] = 0;
  }
  for (int j = 7; j < N; j += 8) {
    free(ptrs[j]);
    ptrs[j] = 0;
  }
  for (int j = 4; j < N; j += 8) {
    free(ptrs[j]);
    ptrs[j] = 0;
  }
  for (int j = 1; j < N; j += 8) {
    free(ptrs[j]);
    ptrs[j] = 0;
  }
  for (int j = 6; j < N; j += 8) {
    free(ptrs[j]);
    ptrs[j] = 0;
  }
  for (int j = 0; j < N; j += 8) {
    free(ptrs[j]);
    ptrs[j] = 0;
  }
  for (int j = 3; j < N; j += 8) {
    free(ptrs[j]);
    ptrs[j] = 0;
  }
  for (int j = 5; j < N; j += 8) {
    free(ptrs[j]);
    ptrs[j] = 0;
  }

  // make large alloc to ensure merging
  ptrs[0] = malloc(1 << 20);
  if (!ptrs[0]) {
    panic("*** unable to malloc large block after test %d\n", test_counter);
  } else {
    free(ptrs[0]);
    ptrs[0] = 0;
  }

  // print results
  if (mCount == fCount + 1) {
    printf("*** test %d counts match\n", test_counter);
  } else {
    printf("*** test %d counts do not match (mallocs: %ld, frees: %ld)\n",
           test_counter, mCount, fCount);
  }
  printf("*** test %d finished\n", test_counter);
  // #endif

  // #ifndef TEST_FOUR
  // #define TEST_FOUR
  //
  // sicko mode
  // we will deallocate and malloc in the holes
  // like filling the center of a bagel!
  test_counter++;

  // populate ptrs
  for (int i = 0; i < N; i++) {
    ptrs[i] = malloc(1);
    if (!check_alignment(ptrs[i])) {
      panic("*** alignment failure at offset %d\n", i);
    }
  }

  // free first two of eight
  for (int j = 0; j < N; j += 8) {
    free(ptrs[j]);
    ptrs[j] = 0;
  }
  for (int j = 1; j < N; j += 8) {
    free(ptrs[j]);
    ptrs[j] = 0;
  }

  // malloc in holes
  for (int i = 0; i < N; i += 8) {
    ptrs[i] = malloc(2);
    if (!check_alignment(ptrs[i])) {
      panic("*** alignment failure at offset %d\n", i);
    }
  }

  // free all
  for (int i = 0; i < N; i++)
    if (ptrs[i]) {
      free(ptrs[i]);
      ptrs[i] = 0;
    }

  // make large alloc to ensure merging
  ptrs[0] = malloc(1 << 20);
  if (!ptrs[0]) {
    panic("*** unable to malloc large block after test %d\n", test_counter);
  } else {
    free(ptrs[0]);
    ptrs[0] = 0;
  }

  // print results
  if (mCount == fCount + 1) {
    printf("*** test %d counts match\n", test_counter);
  } else {
    printf("*** test %d counts do not match (mallocs: %ld, frees: %ld)\n",
           test_counter, mCount, fCount);
  }
  printf("*** test %d finished\n", test_counter);
  // #endif

  return 0;
}
