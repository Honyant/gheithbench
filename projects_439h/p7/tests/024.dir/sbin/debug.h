#pragma once

#include "stdint.h"
#include "libc.h"
#include "sys.h"

inline static void assert(int invariant, const char* invariant_text, const char* file, int line) {
    if (!invariant) {
        printf("*** Assertion [%s] failed at %s:%d\n",invariant_text,file,line);
        shutdown();
    }
}

//inline void ASSERT(bool) {}
#define ASSERT(invariant) assert(invariant,#invariant,__FILE__,__LINE__)