#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "slice.h"
#include <stdint.h>

typedef struct Node{
    struct Node* next;
    Slice* key;
    uint64_t value;
} Node;

typedef struct{
    Node** vals;
    size_t size;
} Hashmap;

Hashmap map_create(int buckets);

void map_insert(Hashmap* hmap, Slice* key, uint64_t value);

uint64_t map_get(Hashmap* hmap, Slice* key);

void map_remove(Hashmap* hmap, Slice* key);

void map_destroy(Hashmap* hmap);