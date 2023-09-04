#pragma once

#include <stddef.h>

#define BYTES (1)
#define KILOBYTES (BYTES * 1000)
#define MEGABYTES (KILOBYTES * 1000)

typedef struct
{
    size_t size;
    size_t offset;
    size_t padding;
} ArenaMemory;

typedef struct
{
    void *head;
    size_t size;
    size_t offset;
    size_t peak;
    size_t padding;
} StackMemory;

typedef struct
{
    void *head;
    size_t padding;
    size_t size;
    size_t capacity;
} PoolMemory;

void format_bytes(double bytes, char *buff, size_t n);
void clear(void *ptr, size_t s);

void *std_alloc(size_t size, int alignment);
void std_free(void **ptr);

ArenaMemory *make_arena_from(void *ptr, size_t size);
ArenaMemory *make_arena(size_t size);
void *arena_alloc(ArenaMemory *self, size_t size, int alignment);
void arena_reset(ArenaMemory *self);
void arena_destroy(ArenaMemory **self);

StackMemory *make_stack_from(void *ptr, size_t size);
StackMemory *make_stack(size_t size);
void *stack_alloc(StackMemory *self, size_t size, int alignment);
void stack_free(StackMemory *self, void **ptr);
void stack_reset(StackMemory *self);
void stack_destroy(StackMemory **self);

PoolMemory *make_pool_from(void *ptr, size_t size, size_t chunkSize);
PoolMemory *make_pool(size_t size, size_t chunkSize);
void *pool_alloc(PoolMemory *self);
void pool_free(PoolMemory *self, void **ptr);
void pool_destroy(PoolMemory **self);