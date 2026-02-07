#include "heap/heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define HEAP_SIZE 1024

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

// ALIGNMENT CHECK
int main() {
  // Malloc pointers of sizes 0 through 16 bytes
  void* pointers[17];
  for (int i = 0; i < 17; i++) {
    void* p = malloc(i);
    pointers[i] = p;
  }

  // Test that each pointer is divisible by 16
  bool somePointerIsMisaligned = false;
  for (int i = 0; i < 17; i++) {
    if (i != 0 && pointers[i] == NULL) {
      printf("Pointer %d is a null pointer.", i);
    }
    if ((uintptr_t)pointers[i] % 16 != 0) {
      printf("Pointer %d is misaligned.", i);
      somePointerIsMisaligned = true;
    }
  }

  // Free the pointers
  for (int i = 0; i < 17; i++) {
    free(pointers[i]);
  }

  if (!somePointerIsMisaligned) {
    printf("All pointers are properly aligned.");
  }

  return 0;
}
