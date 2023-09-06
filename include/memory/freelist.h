#pragma once

#include <stddef.h>

typedef struct
{
	void *next;
	unsigned int size;
	unsigned int padding;
} FreeListMemory;

void *freelist_alloc(FreeListMemory *self, size_t size, unsigned int alignment);
unsigned char freelist_free(FreeListMemory *self, void **ptr);
void freelist_reset(FreeListMemory *self);
void freelist_destroy(FreeListMemory **self);
FreeListMemory *freelist_create(void *m, size_t size);
FreeListMemory *make_freelist(size_t size);