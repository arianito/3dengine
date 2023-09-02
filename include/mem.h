#pragma once

#include <stddef.h>
#include <stddef.h>
#include <malloc.h>

typedef enum
{
    READY,
    INVALID_INPUT,
    INSUFFICIENT_MEMORY,
    ALLOCATION_FAILED,
    INVALID_ALIGNMENT,
    OUT_OF_BOUNDARY,
} error_t;

typedef struct
{
    size_t size;
    size_t offset;
    size_t padding;
} ArenaMemory;

ArenaMemory *arenap(void *m, size_t size, error_t *err);
ArenaMemory *arena(size_t size, error_t *err);
void arena_destroy(ArenaMemory **arena, error_t *err);
void *arena_ptr(ArenaMemory *arena, error_t *err);
void *arena_allocate(ArenaMemory *arena, size_t size, int alignment, error_t *err);
void arena_reset(ArenaMemory *arena);

//

typedef struct
{
    void *head;
    size_t size;
    size_t offset;
    size_t peak;
    size_t padding;
} StackMemory;

StackMemory *stackp(void *m, size_t size, error_t *err);
StackMemory *stack(size_t size, error_t *err);
void stack_destroy(StackMemory **stack, error_t *err);
void *stack_ptr(StackMemory *stack, error_t *err);
void *stack_allocate(StackMemory *stack, size_t size, int alignment, error_t *err);
void stack_free(StackMemory *stack, void **p, error_t *err);
void stack_reset(StackMemory *stack);

//

// void *mpool_create(size_t size, size_t chunk_size, unsigned char alignment);
// void mpool_destroy(void *mem);
// char mpool_check(void *mem);
// void *mpool_alloc(void *mem);
// void mpool_free(void *mem, void *p);
// size_t mpool_space(void *mem);
// void *mpool_resize(void *mem, size_t size);
// void mpool_usage(void *mem);