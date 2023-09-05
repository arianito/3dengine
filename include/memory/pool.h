#pragma once

#include <stddef.h>

typedef struct
{
	void *head;
	size_t size;
	unsigned int padding;
	unsigned int capacity;
} PoolMemory;

typedef struct
{
	size_t data;
} PoolMemoryNode;

PoolMemory *make_pool(size_t size, unsigned int chunkSize);
void *pool_alloc(PoolMemory *self);
unsigned char pool_free(PoolMemory *self, void **ptr);
void pool_destroy(PoolMemory **self);