#pragma once

#include "libc.h"
#include "stdbool.h"
#include "stdint.h"
#include "sys.h"
#include <stdarg.h>

/*
CITATION: From Michael Jennings' edpost, #531. Thank you Michael : )
*/

inline static void assert(bool invariant, const char *invariant_text,
                          const char *file, int line) {
  if (!invariant) {
    printf("*** Assertion [%s] failed at %s:%d\n", invariant_text, file, line);
    shutdown();
  }
}

// inline void ASSERT(bool) {}
#define ASSERT(invariant) assert(invariant, #invariant, __FILE__, __LINE__)