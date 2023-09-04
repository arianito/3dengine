#pragma once

#include <stddef.h>

typedef struct
{
	void *head;
	size_t padding;
	size_t size;
	size_t capacity;
} PoolMemory;

typedef struct
{
	size_t data;
} PoolMemoryNode;

PoolMemory *make_pool_from(void *ptr, size_t size, size_t chunkSize);
PoolMemory *make_pool(size_t size, size_t chunkSize);
void *pool_alloc(PoolMemory *self);
void pool_free(PoolMemory *self, void **ptr);
void pool_destroy(PoolMemory **self);