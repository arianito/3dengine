

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "memory/pool.h"
#include "memory/utils.h"

void pool_enqueue(PoolMemory *self, PoolMemoryNode *node)
{
	size_t start = (size_t)self - self->padding;

	if (self->head == NULL)
		node->data = 0;
	else
		byte7a(&node->data, (size_t)self->head - start, 0);

	self->head = node;
}

PoolMemoryNode *pool_dequeue(PoolMemory *self)
{
	if (self->head == NULL)
		return NULL;
	PoolMemoryNode *node = self->head;
	byte7a1(&node->data, 1);

	size_t offset;
	byte7d(node->data, &offset, NULL);
	size_t start = (size_t)self - self->padding;
	//
	if (offset == 0)
		self->head = NULL;
	else
		self->head = (PoolMemoryNode *)(start + offset);
	//
	return node;
}

void *pool_alloc(PoolMemory *self)
{
	if (self == NULL)
	{
		printf("pool: alloc failed, invalid instance\n");
		exit(EXIT_FAILURE);
		return NULL;
	}
	if (self->head == NULL)
	{
		printf("pool: alloc failed, insufficient memory\n");
		return NULL;
	}
	PoolMemoryNode *node = pool_dequeue(self);
	self->capacity--;
	unsigned int space = calculate_space(sizeof(PoolMemoryNode), sizeof(size_t));
	return (void *)((size_t)node + space);
}

unsigned char pool_free(PoolMemory *self, void **p)
{
	if (self == NULL)
	{
		printf("pool: free failed, invalid instance\n");
		return 0;
	}
	if (p == NULL || (*p) == NULL)
	{
		printf("pool: free failed, invalid pointer\n");
		return 0;
	}
	size_t start = (size_t)self - self->padding;
	size_t address = (size_t)(*p);
	size_t end = start + self->size;

	if (!(address >= start || address < end))
	{
		printf("pool: free failed, out of boundary\n");
		return 0;
	}

	unsigned int space = calculate_space(sizeof(PoolMemoryNode), sizeof(size_t));
	PoolMemoryNode *node = (PoolMemoryNode *)(address - space);

	unsigned char used;
	byte7d(node->data, NULL, &used);
	if (!used)
	{
		printf("pool: free failed, already freed\n");
		return 0;
	}

	pool_enqueue(self, node);
	self->capacity++;
	(*p) = NULL;
	return 1;
}

void pool_destroy(PoolMemory **self)
{
	if (self == NULL || *self == NULL)
	{
		printf("pool: destroy failed, invalid instance\n");
		return;
	}
	size_t op = (size_t)(*self) - (*self)->padding;
	free((void *)(op));
	(*self) = NULL;
}

PoolMemory *make_pool(size_t size, unsigned int chunkSize)
{
	void *m = malloc(size);
	if (m == NULL)
	{
		printf("pool: make failed, system can't provide free memory\n");
		exit(EXIT_FAILURE);
		return NULL;
	}
	size_t start = (size_t)m;
	unsigned int space = calculate_space(sizeof(PoolMemory), sizeof(size_t));
	unsigned int padding = calculate_padding(start, sizeof(size_t));
	PoolMemory *self = (PoolMemory *)(start + padding);
	self->head = NULL;
	self->size = size;
	self->padding = padding;
	self->capacity = 0;
	size_t cursor = padding + space;
	space = calculate_space(sizeof(PoolMemoryNode), sizeof(size_t));
	while (1)
	{
		size_t chunk_address = start + cursor;
		padding = calculate_alignment(chunk_address, sizeof(PoolMemoryNode), sizeof(size_t));
		cursor += padding + chunkSize;
		if (cursor > size)
			break;

		pool_enqueue(self, (PoolMemoryNode *)((chunk_address + padding) - space));
		self->capacity++;
	}
	return self;
}