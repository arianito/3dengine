#pragma once

#include <stddef.h>

typedef struct {
    unsigned int size;
    unsigned char free;
} BuddyBlock;

typedef struct {
    BuddyBlock *head;
    BuddyBlock *tail;
    unsigned int padding;
    unsigned int usage;
} BuddyMemory;

unsigned int buddy_size(unsigned int order);

BuddyMemory *buddy_create(void *m, unsigned int order);

BuddyMemory *make_buddy(unsigned int order);

void buddy_destroy(BuddyMemory **self);

void *buddy_alloc(BuddyMemory *self, unsigned int size);

char buddy_free(BuddyMemory *self, void **ptr);