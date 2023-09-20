
#include "mem/alloc.h"

#include "mem/std.h"

MemoryLayout *alloc = NULL;

void alloc_create(MemoryMetadata meta) {
    alloc = std_alloc(sizeof(MemoryLayout), sizeof(size_t));
    alloc->metadata = meta;
    alloc->global = make_arena(meta.global);
    alloc->stack = stack_create(
            arena_alloc(alloc->global, meta.stack, sizeof(size_t)),
            meta.stack
    );
    alloc->freelist = freelist_create(
            arena_alloc(alloc->global, meta.freelist, sizeof(size_t)),
            meta.freelist
    );
    alloc->string = freelist_create(
            arena_alloc(alloc->global, meta.string, sizeof(size_t)),
            meta.string
    );
}

void alloc_terminate() {
    arena_destroy(&alloc->global);
    std_free((void **) &alloc);
}

void alloc_debug() {
    printf("global: %zu/%zu \n", alloc->global->offset, alloc->global->size);
    printf("stack: %zu/%zu \n", alloc->stack->offset, alloc->stack->size);
    printf("freelist: %zu/%zu \n", freelist_capacity(alloc->freelist), alloc->freelist->size);
    printf("string: %zu/%zu \n", freelist_capacity(alloc->string), alloc->string->size);
    printf("----------- \n");
}