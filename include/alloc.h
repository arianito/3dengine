#pragma once

#include <stdio.h>
#include "stddef.h"
#include "mem.h"

typedef struct
{
    ArenaMemory *global;
    StackMemory *stack;
    PoolMemory *pool64;
    PoolMemory *pool128;
    PoolMemory *pool256;
} MemoryLayout;

ArenaMemory *footprint;
MemoryLayout *memory;

typedef struct
{
    size_t global;
    size_t stack;
    size_t pool64;
    size_t pool128;
    size_t pool256;
} MemoryDef;

void footprint_create(MemoryDef def);
void footprint_terminate();

inline void alloc_debug()
{
    printf("global: %zu/%zu \n", memory->global->offset, memory->global->size);
    printf("stack: %zu/%zu \n", memory->stack->offset, memory->stack->size);
    printf("pool64: %zu/%zu \n", memory->pool64->size - memory->pool64->capacity * 64, memory->pool64->size);
    printf("pool128: %zu/%zu \n", memory->pool128->size - memory->pool128->capacity * 128, memory->pool128->size);
    printf("pool256: %zu/%zu \n", memory->pool256->size - memory->pool256->capacity * 256, memory->pool256->size);
}

inline void *alloc_global(size_t size)
{
    MemoryErrorEnum err = READY;
    void *p = arena_allocate(memory->global, size, sizeof(size_t), &err);
    handler_memory_error(err);
    return p;
}

inline void *alloc_stack(size_t size)
{
    MemoryErrorEnum err = READY;
    void *p = stack_allocate(memory->stack, size, sizeof(size_t), &err);
    handler_memory_error(err);
    return p;
}

inline void free_stack(void *p)
{
    MemoryErrorEnum err = READY;
    stack_free(memory->stack, &p, &err);
    handler_memory_error(err);
}

inline void *alloc_pool64()
{
    MemoryErrorEnum err = READY;
    void *p = pool_allocate(memory->pool64, &err);
    handler_memory_error(err);
    return p;
}

inline void *free_pool64(void *p)
{
    MemoryErrorEnum err = READY;
    pool_free(memory->pool64, &p, &err);
    handler_memory_error(err);
}

inline void *alloc_pool128()
{
    MemoryErrorEnum err = READY;
    void *p = pool_allocate(memory->pool128, &err);
    handler_memory_error(err);
    return p;
}

inline void *free_pool128(void *p)
{
    MemoryErrorEnum err = READY;
    pool_free(memory->pool128, &p, &err);
    handler_memory_error(err);
}

inline void *alloc_pool256()
{
    MemoryErrorEnum err = READY;
    void *p = pool_allocate(memory->pool256, &err);
    handler_memory_error(err);
    return p;
}

inline void *free_pool256(void *p)
{
    MemoryErrorEnum err = READY;
    pool_free(memory->pool256, &p, &err);
    handler_memory_error(err);
}
