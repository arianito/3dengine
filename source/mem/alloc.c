
#include "mem/alloc.h"

#include <math.h>
#include "mem/std.h"

MemoryLayout *alloc = NULL;

void alloc_create(MemoryMetadata meta) {
    alloc = std_alloc(sizeof(MemoryLayout), sizeof(size_t));

    meta.boot += sizeof (ArenaMemory);
    meta.boot += sizeof (StackMemory);
    meta.boot += sizeof (FreeListMemory);
    meta.boot += sizeof (FreeListMemory);
    meta.boot += sizeof (BuddyMemory);

    alloc->metadata = meta;
    alloc->boot = make_arena_exact(meta.boot);

    alloc->global = arena_create(
            arena_alloc(alloc->boot, meta.global, sizeof(size_t)),
            meta.global
    );
    alloc->stack = stack_create(
            arena_alloc(alloc->boot, meta.stack, sizeof(size_t)),
            meta.stack
    );
    alloc->freelist = freelist_create(
            arena_alloc(alloc->boot, meta.freelist, sizeof(size_t)),
            meta.freelist
    );
    alloc->string = freelist_create(
            arena_alloc(alloc->boot, meta.string, sizeof(size_t)),
            meta.string
    );
    unsigned char order = (unsigned char) log2((double )meta.buddy);
    alloc->buddy = buddy_create(
            arena_alloc(alloc->boot, buddy_size(order), sizeof(size_t)),
            order
    );
}

void alloc_terminate() {
    arena_destroy(&alloc->boot);
    std_free((void **) &alloc);
}
