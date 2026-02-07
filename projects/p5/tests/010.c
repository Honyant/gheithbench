#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define BLOCK_SIZE 128
#define FILL_CHAR 0x55
#define ITERATIONS 100000

#define HEAP_SIZE BLOCK_SIZE*4

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

#define M 3
void* last[M] = {}; // can only have 3 blocks ever allocated at a time in the heap

static int check_valid(char* ptr) {
  // makes sure that data has not been touched
  for (int i = 0; i < BLOCK_SIZE; i++) {
    if (ptr[i] != FILL_CHAR) {
      return 0;
    }
  }

  return 1;
}

static void fill(char* ptr) {
  for (int i = 0; i < BLOCK_SIZE; i++) {
    ptr[i] = FILL_CHAR;
  }
}

int main() {
  for (uint32_t i=0; i<M; i++) {
    last[i] = 0;
  }

  long m1 = mCount;
  long f1 = fCount;

  for (uint32_t i=0; i<ITERATIONS; i++) {
    uint32_t x = rand() % M;
    if (last[x] != 0) {
      if (check_valid((char*) last[x]) == 0) {
        panic("*** failure: data was overriden\n");
      }
      free(last[x]);
      last[x] = 0;
    }
    last[x] = malloc(BLOCK_SIZE);
    if (last[x] == 0) {
      panic("*** failed to allocate %d\n", BLOCK_SIZE);
    }
    char* ptr = (char*) last[x];

    fill(ptr);
  }

  for (uint32_t i=0; i<M; i++) {
    if (last[i] != 0) {
      if (check_valid((char*) last[i]) == 0) {
        panic("*** failure: data was overriden\n");
      }
      free(last[i]);
    }
  }

  long m2 = mCount - m1;
  long f2 = fCount - f1;



  if (m2 != f2) {
    printf("m2 %ld\n",m2);
    printf("f2 %ld\n",f2);
  } else {
    printf("count match\n");
  }

  if (m2 != ITERATIONS) {
    printf("*** wrong count %ld\n",m2);
  } else {
    printf("count ok\n");
  }

  return 0;
}
