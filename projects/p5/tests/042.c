#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap/heap.h"
#include "heap/panic.h"

/*
 * include stress test for certain optimizations and stuff, edge cases like malloc 0 and free NULL
 * also break tests into functions, comments, print out debugging messages if what they returned was wrong!
 */

// defines testing global variables
#define HEAP_SIZE (1 << 20) // ~1 million
#define STRESS_TEST_SIZE 21000
#define NORMAL_TEST_SIZE 10000
#define SMALL_TEST_SIZE 1000

long M_COUNT_ANSWER = 0;
long F_COUNT_ANSWER = 0;

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];
void* malloc_testing_array[NORMAL_TEST_SIZE];

/*
 *  EDGE CASES TESTING
 */
void edge_case_testing() {
    // Test malloc with 0 bytes
    void* p = malloc(0); M_COUNT_ANSWER++;

    free(p); F_COUNT_ANSWER++;
    // Test free with NULL
    free(NULL); F_COUNT_ANSWER++;
}
/*
 *  STRESS TESTING
 */
void stress_testing() {
    // Test repeated malloc and free -- created so even 32 headers can work with it
    // logic: 1 << 20 (total bytes) / 8 (per index) / 6 (2header, 2data, 2footer worst case)
    void* p = NULL;
    for (int i = 0; i < STRESS_TEST_SIZE; i++) {
        p = malloc(1);
        if (p == 0) panic("*** failed to allocate 1 byte block in stress test***");
        free(p);
    }
    M_COUNT_ANSWER += STRESS_TEST_SIZE; F_COUNT_ANSWER += STRESS_TEST_SIZE;
    // test repeated malloc of 0 to ensure heap has enough space
    for (int i = 0; i < STRESS_TEST_SIZE; i++) {
        p = malloc(0);
    }
    M_COUNT_ANSWER += STRESS_TEST_SIZE;
}

/*
 *  OPTIMIZATION TESTING
 */
void optimization_testing() {
    // tests mallocing and then freeing in reverse order
    // this is good for doubly linked lists
    for (int i = 0; i < NORMAL_TEST_SIZE; i++) {
        malloc_testing_array[i] = malloc(1);
    }
    M_COUNT_ANSWER += NORMAL_TEST_SIZE;
    for (int i = NORMAL_TEST_SIZE-1; i >= 0; i--) {
        free(malloc_testing_array[i]);
    }
    F_COUNT_ANSWER += NORMAL_TEST_SIZE;

    // tests filling the entire heap
    // this is good for smaller metadata sizes
    for (int i = 0; i < NORMAL_TEST_SIZE; i++) {
        void* large_block = malloc(HEAP_SIZE/2);
        free(large_block);
    }
    M_COUNT_ANSWER += NORMAL_TEST_SIZE; F_COUNT_ANSWER += NORMAL_TEST_SIZE;

    // tests gaps and block merging
    // good for people that merge their free blocks
    for (int i = 0; i < NORMAL_TEST_SIZE; i++) {
        malloc_testing_array[i] = malloc(1);
    }
    M_COUNT_ANSWER += NORMAL_TEST_SIZE;
    for (int i = NORMAL_TEST_SIZE/2-2500; i < NORMAL_TEST_SIZE/2+2500; i++) {
        free(malloc_testing_array[i]); F_COUNT_ANSWER++;
    }
}

/*
 *  RANDOM TESTING
 */
void random_testing() {
    // this tests mallocing random sized blocks
    for (int i = 0; i < SMALL_TEST_SIZE; i++) {
        malloc_testing_array[i] = malloc(rand() % 1000 + 1);
    }
    M_COUNT_ANSWER += SMALL_TEST_SIZE;
    for (int i = SMALL_TEST_SIZE-1; i >= 0; i--) {
        free(malloc_testing_array[i]);
    }
    F_COUNT_ANSWER += SMALL_TEST_SIZE;

    // this tests creates blocks and tries to free them in random order to see if all possible iterations of freeing works
    for (int i = 0; i < 6; i++) {
        void* p1 = malloc(33);
        void* p2 = malloc(33);
        void* p3 = malloc(33);
        M_COUNT_ANSWER += 3;

        switch (i) {
            case 0:
                free(p1); free(p2); free(p3);
                break;
            case 1:
                free(p1); free(p3); free(p2);
                break;
            case 2:
                free(p2); free(p1); free(p3);
                break;
            case 3:
                free(p2); free(p3); free(p1);
                break;
            case 4:
                free(p3); free(p1); free(p2);
                break;
            case 5:
                free(p3); free(p2); free(p1);
                break;
        }
        F_COUNT_ANSWER += 3;
    }
}

// check mCount and fCount and call void* malloc(size_t bytes) and void free(void* p)
int main() {
    edge_case_testing();
    printf("passes edge case testing!\n"); M_COUNT_ANSWER++; // printf uses malloc once!
    stress_testing();
    printf("passes stress testing!\n");
    optimization_testing();
    printf("passes optimization testing!\n");
    random_testing();
    printf("passes randomized testing!\n");

    if (mCount == M_COUNT_ANSWER) {
        printf("m_count count matches!\n");
    } else {
        printf("m_count DOES NOT MATCH, output: %ld, should be: %ld\n",mCount,M_COUNT_ANSWER);
    }

    if (fCount == F_COUNT_ANSWER) {
        printf("f_count matches!\n");
    } else {
        panic("f_count DOES NOT MATCH, output: %ld, should be: %ld\n",fCount,F_COUNT_ANSWER);
    }

    return 0;
}
