
#include "mem/alloc.h"

#include <math.h>
#include "mem/std.h"

MemoryLayout *alloc = NULL;

void *global_slab_alloc(size_t size) {
    return freelist_alloc(alloc->freelist, size, sizeof(size_t));
}

void global_slab_free(void *ptr) {
    freelist_free(alloc->freelist, &ptr);
}

void alloc_create(MemoryMetadata meta) {
    alloc = std_alloc(sizeof(MemoryLayout), sizeof(size_t));

    meta.boot += sizeof(ArenaMemory);
    meta.boot += sizeof(StackMemory);
    meta.boot += sizeof(FreeListMemory);
    meta.boot += sizeof(FreeListMemory);
    meta.boot += sizeof(BuddyMemory);
    meta.boot += sizeof(P2SlabMemory);

    alloc->metadata = meta;
    alloc->boot = make_arena(meta.boot);

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
    unsigned char order = (unsigned char) log2((double) meta.buddy);
    alloc->buddy = buddy_create(
            arena_alloc(alloc->boot, buddy_size(order), sizeof(size_t)),
            order
    );
    GeneralAllocator g = {&global_slab_alloc, &global_slab_free};
    alloc->slab = p2slab_create_alloc(g, 10);

}

void alloc_terminate() {
    arena_destroy(&alloc->boot);
    std_free((void **) &alloc);
}
