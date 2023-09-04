#pragma once

#include <stddef.h>

typedef struct
{
	void *head;
	size_t padding;
	size_t size;
	size_t offset;
} FreeListMemory;

FreeListMemory *make_freelist_from(void *ptr, size_t size, size_t chunkSize);
FreeListMemory *make_freelist(size_t size, size_t chunkSize);
void *freelist_alloc(FreeListMemory *self);
void freelist_free(FreeListMemory *self, void **ptr);
void freelist_reset(FreeListMemory *self);
void freelist_destroy(FreeListMemory **self);