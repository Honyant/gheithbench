#ifndef _HEAP_H_
#define _HEAP_H_

// heap size in bytes, defined by the test
extern long heap_size;

// memory for the heap, defined by the test
// must contain heap_size bytes
extern long the_heap[];

// number of calls to malloc, defined and updated by the heap implementation
extern long mCount;

// number of calls to free, defined and updated by the heap implementation
extern long fCount;

#endif
