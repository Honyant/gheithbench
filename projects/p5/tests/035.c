#include <stdio.h>
#include <stdlib.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define SIZE 65536

long heap_size = SIZE;
long the_heap[SIZE/sizeof(long)];

void check_pointer(void* p) {
    if (p == NULL) {
        printf("malloc() returned null pointer.\n");
    } else if (((long) p) % 16 != 0) {
        printf("malloc() returned misaligned pointer %p.\n", p);
    }
}

void hat_trick() {
    void* pointers[3];
    // merge with next (probably)
    pointers[0] = malloc(1);
    pointers[1] = malloc(1);
    pointers[2] = malloc(1);
    check_pointer(pointers[0]);
    check_pointer(pointers[1]);
    check_pointer(pointers[2]);
    free(pointers[2]);
    free(pointers[1]);
    free(pointers[0]);
    // merge with previous (probably)
    pointers[0] = malloc(1);
    pointers[1] = malloc(1);
    pointers[2] = malloc(1);
    check_pointer(pointers[0]);
    check_pointer(pointers[1]);
    check_pointer(pointers[2]);
    free(pointers[0]);
    free(pointers[1]);
    free(pointers[2]);
    // double merge (probably)
    pointers[0] = malloc(1);
    pointers[1] = malloc(1);
    pointers[2] = malloc(1);
    check_pointer(pointers[0]);
    check_pointer(pointers[1]);
    check_pointer(pointers[2]);
    free(pointers[0]);
    free(pointers[2]);
    free(pointers[1]);
}

void immediate_free() {
    for (int i = 0; i < 64; i++) {
        void* p = malloc((37 * (i + 1)) % 64 + 1); // hits every size 1-64
        check_pointer(p);
        free(p);
    }
}

void inorder_free() {
    void* p[64];
    for (int i = 0; i < 64; i++) {
        p[i] = malloc((37 * (i + 1)) % 64 + 1); // hits every size 1-64
        check_pointer(p[i]);
    }
    for (int i = 0; i < 64; i++) {
        free(p[i]);
    }
}

void reverse_free() {
    void* p[64];
    for (int i = 0; i < 64; i++) {
        p[i] = malloc((37 * (i + 1)) % 64 + 1); // hits every size 1-64
        check_pointer(p[i]);
    }
    for (int i = 63; i >= 0; i--) {
        free(p[i]);
    }
}

void skip_free() {
    void* p[64];
    for (int i = 0; i < 64; i++) {
        p[i] = malloc((37 * (i + 1)) % 64 + 1); // hits every size 1-64
        check_pointer(p[i]);
    }
    for (int i = 63; i >= 0; i--) {
        free(p[(37 * i) % 64]); // frees every pointer once
    }
}

void data_safety() {
    char* p[64];
    char lengths[64];
    for (int i = 0; i < 64; i++) {
        lengths[i] = (37 * (i + 1)) % 64 + 1;
        p[i] = (char*) malloc(lengths[i]);
        check_pointer(p[i]);
        for (char j = 0; j < lengths[i]; j++) {
            p[i][(int) j] = (char) j;
        }
    }
    for (int i = 63; i >= 0; i--) {
        int k = (37 * i) % 64;
        for (int j = 0; j < lengths[k]; j++) {
            if (p[k][j] != j) {
                printf("Hey! Quit tampering with my data!\n");
            }
        }
        free(p[k]); // frees every pointer once
    }
}

void fill_heap() {
    void* p[(heap_size - 64)/64];
    for (int i = 0; i < (heap_size - 64)/64; i++) {
        p[i] = malloc(32);
        check_pointer(p[i]);
    }
    for (int i = 0; i < (heap_size - 64)/64; i++) {
        free(p[i]);
    }
}


void edge_cases() {
    // input edge cases
    long ftemp = fCount;
    free(NULL);
    if (ftemp != fCount - 1) {
        printf("fCount should increment even when freeing NULL.\n");
    }
    fCount--; // otherwise we get an unresolvable discrepancy
    long mtemp = mCount;
    void* p = malloc(0);
    if (mtemp != mCount - 1) {
        printf("mCount should increment even when mallocking 0.\n");
    }
    free(p);
}

int main(int argc, char** argv) {
    hat_trick();
    immediate_free();
    inorder_free();
    reverse_free();
    skip_free();
    data_safety();
    fill_heap();
    edge_cases();

    if (mCount != fCount) {
        printf("mallocs: %ld\n", mCount);
        printf("frees: %ld\n", fCount);
    } else {
        printf("%s", "Congratulations!");
    }
}