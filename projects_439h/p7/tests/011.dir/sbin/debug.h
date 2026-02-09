#pragma once

#include <stdarg.h>
#include "stdint.h"
#include "libc.h"
#include "sys.h"
#include "stdbool.h"

inline static void assert(bool invariant, const char* invariant_text, const char* file, int line) {
    if (!invariant) {
        printf("*** Assertion [%s] failed at %s:%d\n",invariant_text,file,line);
        shutdown();
    }
}

//inline void ASSERT(bool) {}
#define ASSERT(invariant) assert(invariant,#invariant,__FILE__,__LINE__)