#pragma once

#include <stddef.h>

#include "mem/utils.h"

typedef struct __attribute__((aligned(16), packed)) {
    unsigned int total;
    unsigned int usage;
    unsigned int padding;
} ArenaMemory;

ArenaMemory *make_arena(unsigned int size);

ArenaMemory *arena_create(void *m, unsigned int size);

void arena_destroy(ArenaMemory **self);

void *arena_alloc(ArenaMemory *self, unsigned int size, unsigned int alignment);

void arena_reset(ArenaMemory *self);
