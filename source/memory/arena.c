
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "memory/arena.h"
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
	ArenaMemory *instance = self;
	size_t address = ((size_t)instance - instance->padding) + instance->offset;
	int padding = calculate_padding(address, alignment);
	if (instance->offset + size + padding > instance->size)
	{
		printf("arena: alloc failed, insufficient memory\n");
		return NULL;
	}
	address += padding;
	instance->offset += size + padding;
	return (void *)(address);
}

void arena_reset(ArenaMemory *self)
{
	if (self == NULL)
	{
		printf("arena: reset failed, invalid instance\n");
		return;
	}
	ArenaMemory *instance = self;
	unsigned int space = calculate_space(sizeof(ArenaMemory), sizeof(size_t));
	instance->offset = instance->padding + space;
}

void arena_destroy(ArenaMemory **self)
{
	if (self == NULL || *self == NULL)
	{
		printf("arena: destroy failed, invalid instance\n");
		return;
	}
	ArenaMemory *instance = *self;
	size_t op = (size_t)instance - instance->padding;
	free((void *)(op));
	*self = NULL;
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
	size_t address = (size_t)m;
	unsigned int space = calculate_space(sizeof(ArenaMemory), sizeof(size_t));
	unsigned int padding = calculate_padding(address, sizeof(size_t));
	ArenaMemory *instance = (ArenaMemory *)(address + padding);
	instance->size = size;
	instance->offset = padding + space;
	instance->padding = padding;
	return instance;
}
