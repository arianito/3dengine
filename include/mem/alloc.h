#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "mem/memory.h"
#include "mem/arena.h"
#include "mem/stack.h"
#include "mem/freelist.h"

typedef struct {
    size_t global;
    size_t stack;
    size_t freelist;
    size_t string;
} MemoryMetadata;

typedef struct {
    MemoryMetadata metadata;
    ArenaMemory *global;
    StackMemory *stack;
    FreeListMemory *freelist;
    FreeListMemory *string;
} MemoryLayout;

extern MemoryLayout *alloc;

void alloc_create(MemoryMetadata meta);

void alloc_terminate();

void alloc_debug();

#define alloc_global(Type, size) ((Type *)arena_alloc(alloc->global, size, sizeof(size_t)))
#define alloc_stack(Type, size) ((Type *)stack_alloc(alloc->stack, size, sizeof(size_t)))
#define alloc_free(p) (stack_free(alloc->stack, p))