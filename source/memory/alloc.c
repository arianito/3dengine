#include "memory/alloc.h"

#include <malloc.h>

MemoryLayout *alloc = NULL;

void alloc_create(MemoryMetadata meta)
{
    alloc = std_alloc(sizeof(MemoryLayout), sizeof(size_t));
    alloc->metadata = meta;
    alloc->global = make_arena(meta.global);
    alloc->stack = make_stack(meta.stack);
}

void alloc_terminate()
{
    std_free(&alloc);
}

void alloc_debug()
{
    printf("global: %zu/%zu \n", alloc->global->offset, alloc->global->size);
    printf("stack: %zu/%zu \n", alloc->stack->offset, alloc->stack->size);
}