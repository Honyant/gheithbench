#include "slice.h"
#include "hashmap.h"
#include <stdint.h>

Hashmap map_create(int buckets)
{
    Hashmap a;
    a.vals = (Node**) calloc(buckets, sizeof(Node*));
    a.size = buckets;
    return a;
}

void map_insert(Hashmap *hmap, Slice *key, uint64_t value)
{
    int hashVal = slice_Hash(key);
    int idx = hashVal % (hmap->size);

    if (hmap->vals[idx] == NULL)
    { // uninitialized
        Node *a = malloc(sizeof(Node));
        a->key = slice_FromSlice(key);
        a->value = value;
        a->next = NULL;
        hmap->vals[idx] = a;
    }
    else
    {
        Node *currHead = hmap->vals[idx];
        while (1)
        {
            if (slice_EqualsSlice(currHead->key, key))
            { // we at the right node
                currHead->value = value;
                break;
            }
            if (currHead->next == NULL)
            { // we havent discovered the key and it doesnt exist, there is hash collision
                Node *newNode = malloc(sizeof(Node));
                newNode->key = slice_FromSlice(key);
                newNode->value = value;
                newNode->next = NULL;
                currHead->next = newNode;
                break;
            }
            currHead = currHead->next;
        }
    }
}

void map_remove(Hashmap *hmap, Slice *key)
{
    int hashVal = slice_Hash(key);
    int idx = hashVal % (hmap->size);
    if (hmap->vals[idx] == NULL)
    {
        printf("key not found");
    }
    else if (slice_EqualsSlice(hmap->vals[idx]->key, key))
    {
        Node *nextHead = hmap->vals[idx]->next;
        free(hmap->vals[idx]);
        hmap->vals[idx] = nextHead;
    }
    else
    {
        Node *prevHead = hmap->vals[idx];
        Node *currHead = prevHead->next;
        while (currHead != NULL)
        {
            if (slice_EqualsSlice(currHead->key, key))
            {
                Node *nextHead = currHead->next;
                prevHead->next = nextHead;
                free(currHead);
                break;
            }
            prevHead = currHead;
            currHead = currHead->next;
        }
    }
}

uint64_t map_get(Hashmap *hmap, Slice *key)
{
    int hashVal = slice_Hash(key);
    int idx = hashVal % (hmap->size);
    if (hmap->vals[idx] == NULL)
        return 0;
    else
    {
        Node *currHead = hmap->vals[idx];
        while (1)
        {
            if (slice_EqualsSlice(currHead->key, key))
            { // we at the right node
                return currHead->value;
            }
            if (currHead->next == NULL)
            { // we havent discovered the key and it doesnt exist, there is hash collision
                break;
            }
            currHead = currHead->next;
        }
    }
    return 0;
}

void map_destroy(Hashmap *hmap)
{
    for (int i = 0; i < hmap->size; i++)
    {
        if (hmap->vals[i] != NULL)
        {
            Node *currHead = hmap->vals[i];
            while (currHead != NULL)
            {
                Node *nextHead = currHead->next;
                free(currHead->key);
                free(currHead);
                currHead = nextHead;
            }
        }
    }
    free(hmap->vals);
}
