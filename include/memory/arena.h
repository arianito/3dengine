#pragma once

#include <stddef.h>

typedef struct
{
	size_t size;
	size_t offset;
	size_t padding;
} ArenaMemory;

ArenaMemory *make_arena_from(void *ptr, size_t size);
ArenaMemory *make_arena(size_t size);
void *arena_alloc(ArenaMemory *self, size_t size, int alignment);
void arena_reset(ArenaMemory *self);
void arena_destroy(ArenaMemory **self);