#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "heap/heap.h"
#include "heap/panic.h"

void alignment_check(void *);

#define HEAP_SIZE (1 << 20);
long heap_size = HEAP_SIZE;

long the_heap[(1 << 20)/sizeof(long)];

int main(){
	/*
	 * Test 1
	 * Edge cases of malloc include 
	 *		1. malloc(0), and if a pointer is returned, trying to write to it
	 *		2. malloc(more size than you have);
	 *			2a. this can be achieved by trying to malloc one big chunk or 
	 *			2b. filling the heap and then mallocing more
	 *				2bi. freeing the heap and trying the malloc again. this tests free
	*/
	// Test 1 part 1
	void * malloc_zero = malloc(0);
	if (malloc_zero != NULL){
		alignment_check(malloc_zero);
		free(malloc_zero);
	}

	// Test 1 part 2a
	void * malloc_too_much = malloc(1 << 21);
	if (malloc_too_much != NULL){
		printf("If someone tries to malloc more space than in the heap, you should return a null pointer.");
		return 0;
	}
	
	// Test 1 part 2b
	void * malloc_almost_everything = malloc(1 << 19);
	malloc_too_much = malloc((1 << 19) + 10);;
	if (malloc_too_much != NULL){
		printf("If there isn't enough space to malloc, you should return a null pointer");
		return 0;
	}

	// Test 1 part 2bi
	free(malloc_almost_everything); 
	malloc_too_much = malloc((1 << 19) + 10);
	if (malloc_too_much == NULL){
		printf("There is a problem with the free");
		return 0;
	}
	free(malloc_too_much);

	/*
	 * Test 2
	 *	Can write and free from memory
	 */
	int * write_test = (int*) malloc(8);
	*write_test = 12345;
	if (*write_test != 12345)
		printf("there was an error writing to memory.");
	free(write_test);

	/*
	* Test 3 
	* Lightweight stress test for memory writing and alignment.
	*	1. mallocs a bunch of 8B chunks, makes sure they're aligned, tries writing to them
	*	2. mallocs variable sized chunks (1B - 1000B) to make sure they're all aligned properly
	*/
	// Test 3 part 1
	int * pointers[1000];
	for (int i = 0; i < 1000; i++){
		pointers[i] = (int*) malloc(8);
		alignment_check(pointers[i]);
		*pointers[i] = i;
		if (*pointers[i] != i)
			printf("there was an issue writing to memory");	
	}	
	for (int i = 0; i < 1000; i++){
		free(pointers[i]);
	}

	// Test 3 part 2
	void * pointers_2[1000];
	for (int i = 0; i < 1000; i++){
		pointers_2[i] = malloc(i);
		alignment_check(pointers_2[i]);
	}
	for (int i = 0; i < 1000; i++){
		free(pointers_2[i]);
	}


	/*
	* Test 4 
	* Free combines blocks if able
	*	1. <free space> block <free space> ===> <free space>
	*	2. <taken space> block <free space> ===> <taken space> <free space>
	*	3. <free space> block <taken space> ===> <free space> <taken space>
	*	4. <taken space> block <taken spce> ===> <taken space> <free space> <taken space>
	*/
	void *a; 
	void *b;
	void *c;
	void *d;
	// Test 4 part 1
	// this almost fills the heap in thirds
	a = malloc(349000);
	b = malloc(349000);
	c = malloc(349000);
	free(a);
	free(c);
	free(b); // should merge on this free 
	d = malloc(1000000); // this is only possible if the merge happens properly 
	if (d == NULL){
		printf("there is an issue in the merge test 4 part 1.");
		return 0;
	}
	free(d);

	// test 4 part 2 
	a = malloc(349000);
	b = malloc(349000);
	c = malloc(349000);
	free(c);
	free(b);
	d = malloc(600000); // this is only possible if merge happens properly
	if (d == NULL){
		printf("there is an issue in the merge test 4 part 2");
		return 0;
	}
	free(a);
	free(d);

	// test 4 part 3 
	a = malloc(349000);
	b = malloc(349000);
	c = malloc(349000);
	free(a);
	free(b); 
	d = malloc(600000); // this is only possible if the merge happens properly 
	if (d == NULL){
		printf("there is an issue in the merge test 4 part 3");
		return 0;
	}
	free(d);
	free(c);

	// test 4 part 4
	a = malloc(349000);
	b = malloc(349000);
	c = malloc(349000);
	free(b);
	d = malloc(349000);
	if (d == NULL){
		printf("there is a problem with free test 4 part 4");
		return 0;
	}
	free(d);
	free(a);
	free(c);

	printf("Good job! Go get some tacos.\n");
}

void alignment_check(void * ptr){
	uint64_t address = (uint64_t) ptr;
	bool aligned = address % 16 == 0; 
	if (!aligned)
		printf("you have an unaligned pointer");
} 
