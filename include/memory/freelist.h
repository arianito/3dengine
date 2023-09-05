#pragma once

#include <stddef.h>

typedef struct
{
	void *head;
	size_t size;
	size_t padding;
} FreeListMemory;

typedef struct
{
	void *next;
	size_t data; // 6bytes size 2byte padding
} FreeListMemoryNode;

void *freelist_alloc(FreeListMemory *self, size_t size, unsigned int alignment);
unsigned char freelist_free(FreeListMemory *self, void **ptr);
void freelist_reset(FreeListMemory *self);
void freelist_destroy(FreeListMemory **self);
FreeListMemory *freelist_create(void *m, size_t size);
FreeListMemory *make_freelist(size_t size);