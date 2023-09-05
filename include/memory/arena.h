#pragma once

#include <stddef.h>

typedef struct
{
	size_t size;
	size_t offset;
	unsigned int padding;
} ArenaMemory;

ArenaMemory *make_arena(size_t size);
void *arena_alloc(ArenaMemory *self, size_t size, unsigned int alignment);
void arena_reset(ArenaMemory *self);
void arena_destroy(ArenaMemory **self);