#pragma once

#include <stddef.h>
#include <stddef.h>
#include <malloc.h>

#define BYTES (1)
#define KILOBYTES (BYTES * 1024)
#define MEGABYTES (KILOBYTES * 1024)

typedef enum
{
    READY,
    INVALID_INPUT,
    INSUFFICIENT_MEMORY,
    INVALID_ALIGNMENT,
    OUT_OF_BOUNDARY,
} MemoryErrorEnum;

void handler_memory_error(MemoryErrorEnum err);
void format_bytes(double bytes, char *buff, size_t n);
void clear(void *ptr, size_t s);

typedef struct
{
    size_t size;
    size_t offset;
    size_t padding;
} ArenaMemory;

ArenaMemory *arenap(void *m, size_t size, MemoryErrorEnum *err);
ArenaMemory *arena(size_t size, MemoryErrorEnum *err);
void arena_destroy(ArenaMemory **instance, MemoryErrorEnum *err);
void *arena_allocate(ArenaMemory *instance, size_t size, int alignment, MemoryErrorEnum *err);
void arena_reset(ArenaMemory *instance);

//

typedef struct
{
    void *head;
    size_t size;
    size_t offset;
    size_t peak;
    size_t padding;
} StackMemory;

StackMemory *stackp(void *m, size_t size, MemoryErrorEnum *err);
StackMemory *stack(size_t size, MemoryErrorEnum *err);
void stack_destroy(StackMemory **instance, MemoryErrorEnum *err);
void *stack_allocate(StackMemory *instance, size_t size, int alignment, MemoryErrorEnum *err);
void stack_free(StackMemory *instance, void **p, MemoryErrorEnum *err);
void stack_reset(StackMemory *instance);

typedef struct
{
    void *head;
    size_t padding;
    size_t size;
    size_t capacity;
} PoolMemory;

PoolMemory *poolp(void *m, size_t size, size_t chunkSize, MemoryErrorEnum *err);
PoolMemory *pool(size_t size, size_t chunkSize, MemoryErrorEnum *err);
void pool_destroy(PoolMemory **instance, MemoryErrorEnum *err);
void *pool_allocate(PoolMemory *instance, MemoryErrorEnum *err);
void pool_free(PoolMemory *instance, void **p, MemoryErrorEnum *err);

// void *mpool_create(size_t size, size_t chunk_size, unsigned char alignment);
// void mpool_destroy(void *mem);
// char mpool_check(void *mem);
// void *mpool_alloc(void *mem);
// void mpool_free(void *mem, void *p);
// size_t mpool_space(void *mem);
// void *mpool_resize(void *mem, size_t size);
// void mpool_usage(void *mem);