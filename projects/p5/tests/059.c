#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define HEAP_SIZE (1 << 20)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

typedef struct weird_type{
    long a;
    char str[4];
} weird_type;

// Check if addresses are byte alligned
// Check if values don't get overwritten by malloc
// Check if malloc returns a null pointer when heap is full
int main() {
    // Check if addresses are byte alligned
    void *ptr = malloc(1);
    void *ptr1 = malloc(5);
    void *ptr2 = malloc(15);
    void *ptr3 = malloc(3);

    printf("ptr is byte alligned: %d\n", (uint64_t) ptr % 16 == 0);
    printf("ptr1 is byte alligned: %d\n", (uint64_t) ptr1 % 16 == 0);
    printf("ptr2 is byte alligned: %d\n", (uint64_t) ptr2 % 16 == 0);
    printf("ptr3 is byte alligned: %d\n", (uint64_t) ptr3 % 16 == 0);

    free(ptr);
    free(ptr1);
    free(ptr2);
    free(ptr3);

    weird_type *weird_types[30] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    bool stuff_present[30] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    bool test_passed = true;
    for (int i = 0; i < 10000; i ++) {
        int idx_to_remove = rand() % 30;
        if (stuff_present[idx_to_remove]){
            free(weird_types[idx_to_remove]);
        }
        stuff_present[idx_to_remove] = false;

        int idx = i % 30;
        if (i >= 30 && stuff_present[idx]) {
            bool data_not_changed = weird_types[idx]->a == (i - 30);
            char ref[4] = "ABCD";
            for (int j = 0; j < 4; j ++){
                if (weird_types[idx]->str[j] != ref[j]) {
                    data_not_changed = false;
                    break;
                }
            }
            if (!data_not_changed) {
                test_passed = false;
                break;
            }
        }
        weird_types[idx] = malloc(sizeof(weird_type));
        if ((uint64_t) weird_types[idx] % 16 != 0) {
            test_passed = false;
            break;
        }
        *weird_types[idx] = (weird_type) {i, "ABCD"};
        stuff_present[idx] = true;
    }

    printf("Consistency Test: %d\n", test_passed);

    printf("Malloc of size too large results in null pointer: %d\n", malloc(1 << 25) == NULL);
}