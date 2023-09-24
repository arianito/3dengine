#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>


#include "mem/arena.h"
#include "mem/stack.h"
#include "mem/freelist.h"
#include "mem/buddy.h"
#include "mem/utils.h"

typedef struct {
    unsigned int boot;
    unsigned int global;
    unsigned int stack;
    unsigned int freelist;
    unsigned int string;
    unsigned int buddy;
} MemoryMetadata;

typedef struct {
    MemoryMetadata metadata;
    ArenaMemory *boot;
    ArenaMemory *global;
    StackMemory *stack;
    FreeListMemory *freelist;
    FreeListMemory *string;
    BuddyMemory *buddy;
} MemoryLayout;

extern MemoryLayout *alloc;

void alloc_create(MemoryMetadata meta);

void alloc_terminate();

void alloc_debug();

#define alloc_global(Type, size) ((Type *)arena_alloc(alloc->global, size, sizeof(size_t)))
#define alloc_stack(Type, size) ((Type *)stack_alloc(alloc->stack, size, sizeof(size_t)))
#define alloc_free(p) (stack_free(alloc->stack, p))