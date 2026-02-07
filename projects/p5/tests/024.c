#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "heap/heap.h"
#include "heap/panic.h"

#define HEAP_SIZE (1 << 20)

long heap_size = HEAP_SIZE;
long the_heap[HEAP_SIZE/sizeof(long)];

int main() {

    //this test is intended to produce output with the malloc calls
    //we malloc a series of char pointers and write into them

    char* word = malloc(sizeof(char) * 10);
    memcpy(word, "abcde", 6);

    //verify pointer alignment for initial malloc call (checks initialization)
    if ((long)word%16 == 0) {
        printf("pointer on first malloc call is aligned properly\n");
    }

    printf("%s\n", word);

    char* word2 = malloc(sizeof(char) * 39);
    memcpy(word2, "fghij", 6);

    //verify pointer alignment on a subsequent malloc call
    if ((long)word2%16 == 0) {
        printf("pointer on second malloc call is aligned properly\n");
    }

    printf("%s\n", word2);

    char* word3 = malloc(sizeof(char) * 10);
    memcpy(word3, "klmno", 6);

    printf("%s\n", word3);
 
    free(word2);

    //word2 is freed, so we have a free block there where the next char pointer can be malloced
    //checks that the resulting char pointer is validly allocated and byte aligned.

    char* word4 = malloc(sizeof(char) * 6);
    memcpy(word4, "pqrst", 6);

    if ((long)word4%16 == 0) {
        printf("pointer on second malloc call is aligned properly\n");
    }

    printf("%s\n", word4);

    char* word5 = malloc(sizeof(char) * 3);
    memcpy(word5, "a", 2);

    printf("%s\n", word5);

    free(word4);

    //let's make sure we did not overwrite any allocated and unfreed memory by re-printing some values
    printf("%s\n", word3);
    printf("%s\n", word);

    return 0;
}
