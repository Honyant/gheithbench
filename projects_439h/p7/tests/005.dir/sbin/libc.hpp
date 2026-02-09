#ifndef _LIBC_H_
#define _LIBC_H_

#include "sys.h"

#define MISSING() do { \
    putstr("\n*** missing code at"); \
    putstr(__FILE__); \
    putdec(__LINE__); \
} while (0)

extern "C" void* malloc(size_t size);
extern "C" void free(void*);
extern "C" void* realloc(void* ptr, size_t newSize);

void* memset(void* p, int val, size_t sz);
void* memcpy(void* dest, void* src, size_t n);

extern "C" int putchar(int c);
extern "C" int puts(const char *p);

extern "C" int isdigit(int c);
extern "C" int printf(const char* fmt, ...);

extern "C" void cp(int from, int to);

#endif
