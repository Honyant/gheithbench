#include "slice.h"
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

Slice *slice_FromSlice(const Slice *original)
{
    Slice temp = {original->start, original->len};
    Slice *newSlice = (Slice *)malloc(sizeof(Slice));
    memcpy(newSlice, &temp, sizeof(Slice));
    return newSlice;
}

Slice slice_FromStartEnd(char const *const start, char const *const end)
{
    Slice a = {start, (size_t)(end - start)};
    return a;
}

bool slice_EqualsStr(Slice *slice, char const *p)
{
    for (size_t i = 0; i < slice->len; i++)
    {
        if (p[i] != slice->start[i])
            return 0;
    }
    return 1;
}

bool slice_EqualsSlice(Slice *slice, Slice *other)
{
    if (slice->len != other->len)
        return 0;
    for (size_t i = 0; i < slice->len; i++)
    {
        if (other->start[i] != slice->start[i])
            return 0;
    }
    return 1;
}

bool slice_IsIdentifier(Slice *slice)
{
    if (slice->len == 0)
        return 0;
    if (!isalpha(slice->start[0]))
        return 0;
    for (size_t i = 1; i < slice->len; i++)
    {
        if (!isalnum(slice->start[i]))
            return 0;
    }
    return 1;
}

void slice_Print(Slice *slice)
{
    for (size_t i = 0; i < slice->len; i++)
    {
        printf("%c", slice->start[i]);
    }
}

size_t slice_Hash(Slice *key)
{
    size_t out = 5381;
    for (size_t i = 0; i < key->len; i++)
    {
        char const c = key->start[i];
        out = out * 33 + c;
    }
    return out;
}

// // vim: ts=4 sw=2 et
