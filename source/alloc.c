#include "alloc.h"
#include "mem.h"
#include <stdlib.h>

// remove this
#include "stdio.h"

#define MIN_MEMORY 1024

void alloc_create(MemoryDef def)
{
    void *ptr;
    MemoryErrorEnum err = READY;

    size_t size = MIN_MEMORY;
    size += def.global;
    size += def.stack;
    size += def.pool64;
    size += def.pool128;
    size += def.pool256;

    // layout
    footprint = arena(size, &err);
    handler_memory_error(err);

    // memory
    memory = (MemoryLayout *)arena_allocate(footprint, sizeof(MemoryLayout), sizeof(size_t), &err);
    handler_memory_error(err);

    // global
    ptr = arena_allocate(footprint, def.global, sizeof(size_t), &err);
    handler_memory_error(err);

    memory->global = arenap(ptr, def.global, &err);
    handler_memory_error(err);

    // stack
    ptr = arena_allocate(footprint, def.stack, sizeof(size_t), &err);
    handler_memory_error(err);

    memory->stack = stackp(ptr, def.stack, &err);
    handler_memory_error(err);

    // pool64
    ptr = arena_allocate(footprint, def.pool64, sizeof(size_t), &err);
    handler_memory_error(err);

    memory->pool64 = poolp(ptr, def.pool64, 64, &err);
    handler_memory_error(err);

    // pool128
    ptr = arena_allocate(footprint, def.pool128, sizeof(size_t), &err);
    handler_memory_error(err);

    memory->pool128 = poolp(ptr, def.pool128, 128, &err);
    handler_memory_error(err);

    // pool256
    ptr = arena_allocate(footprint, def.pool256, sizeof(size_t), &err);
    handler_memory_error(err);

    memory->pool256 = poolp(ptr, def.pool256, 256, &err);
    handler_memory_error(err);
}

void alloc_terminate()
{
    MemoryErrorEnum err = READY;

    arena_destroy(&footprint, &err);
    handler_memory_error(err);

    exit(1);
}