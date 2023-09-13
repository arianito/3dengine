#pragma once

typedef struct {
    void *next;
    unsigned int order;
    char status;
} BuddyMemory;

void *buddy_alloc(BuddyMemory *self, size_t size);

unsigned char buddy_free(BuddyMemory *self, void **ptr);

void buddy_reset(BuddyMemory *self);

void buddy_destroy(BuddyMemory **self);

BuddyMemory *buddy_create(void *m, unsigned char level);

BuddyMemory *make_buddy(unsigned char level);