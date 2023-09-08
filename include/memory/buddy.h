#pragma once

typedef struct BuddyMemory
{
	void *next;
	unsigned int used;
};

void *buddy_alloc(BuddyMemory *self, size_t size);
unsigned char buddy_free(BuddyMemory *self, void **ptr);
void buddy_reset(BuddyMemory *self);
void buddy_destroy(BuddyMemory **self);
BuddyMemory *buddy_create(void *m, size_t size);
BuddyMemory *make_buddy(size_t size);