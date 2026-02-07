#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

// A slice represents an immutable substring.
// Assumptions:
//      * the underlying string outlives the slice
//      * the underlying string is long enough
//      * the underlying string can be represented with single byte
//        characters (e.g. ASCII)
//
//      Slice representing "cde"
//          +---+---+
//          | o | 3 |
//          +-|-+---+
//            |
//            v
//       ...abcdefg...
//
// This class is intended as a light-weight wrapper around pointer and length
// and should be passed around by value.
//
typedef struct {
    char const *const start;  // where does the string start in memory?
                              // The 2 uses of 'const' express different ideas:
                              //    * the first says that we can't change the
                              //      character we point to
                              //    * the second says that we can't change the
                              //      place we point to
    int64_t const len;        // How many characters in the string
} Slice;

Slice *slice_FromSlice(const Slice *original);

Slice slice_FromStartEnd(char const *const start, char const *const end);

bool slice_EqualsStr(Slice *slice, char const *p);

bool slice_EqualsSlice(Slice *slice, Slice *other);

bool slice_IsIdentifier(Slice *slice);

void slice_Print(Slice *slice);

size_t slice_Hash(Slice *key);


// // vim: ts=4 sw=2 et
