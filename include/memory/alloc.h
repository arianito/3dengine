#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "memory/memory.h"

typedef struct
{
    size_t global;
    size_t stack;
    size_t components;
} MemoryMetadata;

typedef struct
{
    MemoryMetadata metadata;
    ArenaMemory *global;
    StackMemory *stack;

} MemoryLayout;

extern MemoryLayout *alloc;

void alloc_create(MemoryMetadata meta);
void alloc_terminate();
void alloc_debug();

#define alloc_global(Type) ((Type *)arena_alloc(alloc->global, sizeof(Type), sizeof(size_t)))
#define alloc_stack(Type) ((Type *)stack_alloc(alloc->stack, sizeof(Type), sizeof(size_t)))
#define alloc_stack_size(Type, size) ((Type *)stack_alloc(alloc->stack, size, sizeof(size_t)))
#define alloc_free(p) (stack_free(alloc->stack, &p))