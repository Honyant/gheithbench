#include "heap/heap.h"
#include "heap/panic.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define HEAP_SIZE (1 << 25)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE / sizeof(long)];

void setZero(char **ptr, int size) {
  for (int i = 0; i < size; i++) {
    ptr[i] = 0;
  }
}

void freeArray(char **ptr, int size) {
  for (int i = 0; i < size; i++) {
    ptr[i] = 0;
  }
}
int main() {
  // Test 0a: Malloc 0 when array is full
  char *ptrm1 = malloc(HEAP_SIZE - 64);
  if (!ptrm1) {
    panic("Test 0a: Should be able to malloc (heap_size - 64) bytes\n");
  }
  char *otherptrm1 = malloc(0);
  free(otherptrm1);
  free(ptrm1);
  printf("Test 0a: pass\n");

  // Test 0b: Mallocing huge values and freeing them repeatedly.
  long long constant0 = (2LL << 20);
  char *ptr0 = malloc(constant0);
  for (int i = 0; i < 1000; i++) {
    if (ptr0 == 0) {
      panic("Test 0b: Should be able to repeatedly malloc large values\n");
    }
    free(ptr0);
    ptr0 = malloc(constant0);
  }
  free(ptr0);
  printf("Test 0b: pass\n");

  // Test 1: Testing time complexity for allocing and freeing after a lot of
  // memory has been malloced
  int constant1 = 1000;
  char *pointers1[constant1];
  setZero(pointers1, constant1);
  for (int i = 0; i < constant1; i++) {
    uint32_t x = rand() % constant1;
    if (pointers1[x] != 0) {
      free(pointers1[x]);
      pointers1[x] = 0;
    }
    pointers1[x] = malloc(constant1);
  }

  char *ptr1 = malloc(10000);
  for (int i = 0; i < constant1; i++) {
    if (ptr1 == 0) {
      panic("Test 1: Should be able to malloc fast after lots of memory "
            "additions\n");
    }
    free(ptr1);
    ptr1 = malloc(1000);
  }
  free(ptr1);
  freeArray(pointers1, constant1);
  printf("Test 1: pass\n");

  // Test 2: Check that all areas of a mallocced portion are preserved.
  int constant2 = 1000;
  char *pointers2[constant2];
  setZero(pointers2, constant2);
  uint8_t *ptr2 = malloc(constant2);
  if (!ptr2) {
    panic("Test 2: Should be able to allocate memory here\n");
  }

  // Writing into ptr2; checking later that it is preserved
  for (int i = 0; i < constant2; i++) {
    ptr2[i] = i % 256;
  }

  for (int i = 0; i < constant2; i++) {
    uint32_t x = rand() % constant2;
    if (pointers2[x] != 0) {
      free(pointers2[x]);
      pointers2[x] = 0;
    }
    uint32_t sz = rand() % constant2;
    pointers2[x] = malloc(sz);
    if (!pointers2[x]) {
      panic("Test 2: Should have space\n");
    }
  }
  freeArray(pointers2, constant2);
  for (int i = 0; i < constant2; i++) {
    if (ptr2[i] != (i % 256)) {
      panic("Test 2: Did not preserve values\n");
    }
  }
  free(ptr2);
  printf("Test 2: pass\n");

  // Test 3: Malloc 0 stress test;
  char *ptr3 = malloc(0);

  for (int i = 0; i < 10000; i++) {
    free(ptr3);
    ptr3 = malloc(0);
  }
  free(ptr3);
  printf("Test 3: pass\n");

  // Test 4: External Fragmentation with powers of 2 minus 1
  int pwrs2 = 15;
  long long arr[pwrs2];
  int constant4 = 2000;
  char *pointers4[constant4];
  setZero(pointers4, constant4);
  arr[0] = 1;
  for (int i = 1; i < pwrs2; i++) {
    arr[i] = 2 * (arr[i-1]) + 1;
  }
  for (int i = 0; i < constant4; i++) {
    uint32_t x = rand() % constant4;
    if (pointers4[x] != 0) {
      free(pointers4[x]);
      pointers4[x] = 0;
    }
    uint32_t sz = arr[rand() % pwrs2];
    pointers4[x] = malloc(sz);
    if (!pointers4[x]) {
      panic("Test 4: Should have space\n");
    }
  }
  freeArray(pointers4, constant4);
  printf("Test 4: pass\n");
}