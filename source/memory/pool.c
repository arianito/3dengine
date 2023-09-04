

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "memory/pool.h"
#include "memory/utils.h"

void *pool_alloc(PoolMemory *self)
{
	if (self == NULL)
	{
		printf("pool: invalid instance\n");
		exit(EXIT_FAILURE);
		return NULL;
	}
	if (self->head == NULL)
	{
		printf("pool: insufficient memory\n");
		return NULL;
	}
	size_t start = (size_t)self - self->padding;
	PoolMemoryNode *node = self->head;
	// dequeue
	size_t offset;
	char used;
	byte7d(node->data, &offset, &used);
	int poolSpace = calculate_space(sizeof(PoolMemory), sizeof(size_t));
	if (offset == self->padding + poolSpace)
		self->head = NULL;
	else
		self->head = (void *)(start + offset);
	byte7a(&node->data, offset, 1);
	//
	self->capacity--;
	int space = calculate_space(sizeof(PoolMemoryNode), sizeof(size_t));
	return (void *)((size_t)node + space);
}

void pool_free(PoolMemory *self, void **p)
{
	if (self == NULL || p == NULL || (*p) == NULL)
		return;
	size_t start = (size_t)self - self->padding;
	size_t address = (size_t)(*p);
	size_t end = start + self->size;

	if (!(address >= start || address < end))
		return;

	int space = calculate_space(sizeof(PoolMemoryNode), sizeof(size_t));
	PoolMemoryNode *node = (PoolMemoryNode *)(address - space);
	// node->next = instance->head;
	// enqueue
	size_t offset;
	char used;
	byte7d(node->data, &offset, &used);
	if (used)
		return;
	int poolSpace = calculate_space(sizeof(PoolMemory), sizeof(size_t));
	if (self->head == NULL)
		byte7a(&node->data, self->padding + poolSpace, 0);
	else
		byte7a(&node->data, ((size_t)self->head - start), 0);
	// --
	self->head = node;
	self->capacity++;
	(*p) = NULL;
}

void pool_destroy(PoolMemory **self)
{
	if (self == NULL || *self == NULL)
		return;
	size_t op = (size_t)(*self) - (*self)->padding;
	free((void *)(op));
	(*self) = NULL;
}

PoolMemory *make_pool_from(void *m, size_t size, size_t chunkSize)
{
	size_t address = (size_t)m;
	int space = calculate_space(sizeof(PoolMemory), sizeof(size_t));
	int padding = calculate_padding(address, sizeof(size_t));
	PoolMemory *self = (PoolMemory *)(address + padding);
	self->head = NULL;
	self->size = size;
	self->padding = padding;
	self->capacity = 0;
	size_t cursor = padding + space;
	while (1)
	{
		size_t chunk_address = address + cursor;
		padding = calculate_alignment(chunk_address, sizeof(PoolMemoryNode), sizeof(size_t));
		space = calculate_space(sizeof(PoolMemoryNode), sizeof(size_t));
		if (cursor + padding + chunkSize > size)
			break;
		cursor += padding + chunkSize;
		chunk_address += padding;
		PoolMemoryNode *node = (PoolMemoryNode *)(chunk_address - space);
		// node->next = instance->head;
		// enqueue
		if (self->head != NULL)
			byte7a(&node->data, ((size_t)self->head - address), 0);
		else
			byte7a(&node->data, cursor, 0);
		// --
		self->head = node;
		self->capacity++;
	}

	return self;
}

PoolMemory *make_pool(size_t size, size_t chunkSize)
{
	void *m = malloc(size);
	if (m == NULL)
	{
		printf("pool: system can't provide free memory\n");
		exit(EXIT_FAILURE);
		return NULL;
	}
	return make_pool_from(m, size, chunkSize);
}