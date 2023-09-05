#include "memory/arena.h"

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "memory/utils.h"

void *arena_alloc(ArenaMemory *self, size_t size, unsigned int alignment)
{
	if ((alignment & (alignment - 1)) != 0)
	{
		printf("arena: alloc failed, invalid alignment\n");
		return NULL;
	}
	if (self == NULL)
	{
		printf("arena: alloc failed, invalid instance\n");
		return NULL;
	}
	size_t address = ((size_t)self - self->padding) + self->offset;
	int padding = calculate_padding(address, alignment);
	if (self->offset + size + padding > self->size)
	{
		printf("arena: alloc failed, insufficient memory\n");
		return NULL;
	}
	address += padding;
	self->offset += size + padding;
	return (void *)(address);
}

void arena_reset(ArenaMemory *self)
{
	if (self == NULL)
	{
		printf("arena: reset failed, invalid instance\n");
		return;
	}
	unsigned int space = calculate_space(sizeof(ArenaMemory), sizeof(size_t));
	self->offset = self->padding + space;
}

void arena_destroy(ArenaMemory **self)
{
	if (self == NULL || *self == NULL)
	{
		printf("arena: destroy failed, invalid instance\n");
		return;
	}
	size_t op = (size_t)(*self) - (*self)->padding;
	free((void *)(op));
	*self = NULL;
}

ArenaMemory *arena_create(void *m, size_t size)
{
	size_t address = (size_t)m;
	unsigned int space = calculate_space(sizeof(ArenaMemory), sizeof(size_t));
	unsigned int padding = calculate_padding(address, sizeof(size_t));
	ArenaMemory *self = (ArenaMemory *)(address + padding);
	self->size = size;
	self->offset = padding + space;
	self->padding = padding;
	return self;
}

ArenaMemory *make_arena(size_t size)
{
	void *m = malloc(size);
	if (m == NULL)
	{
		printf("arena: make failed, system can't provide free memory\n");
		exit(EXIT_FAILURE);
		return NULL;
	}
	return arena_create(m, size);
}
