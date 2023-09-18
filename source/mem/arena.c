
#include "mem/arena.h"

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "mem/utils.h"

void *arena_alloc(ArenaMemory *self, size_t size, unsigned int alignment) {
    if (!ISPOW2(alignment)) {
        printf("arena: alloc failed, invalid alignment\n");
        return NULL;
    }
    if (self == NULL) {
        printf("arena: alloc failed, invalid instance\n");
        return NULL;
    }
    size_t address = ((size_t) self - self->padding) + self->offset;
    int padding = MEMORY_PADDING(address, alignment);
    if (self->offset + size + padding > self->size) {
        printf("arena: alloc failed, insufficient memory\n");
        return NULL;
    }
    address += padding;
    self->offset += size + padding;
    return (void *) (address);
}

void arena_reset(ArenaMemory *self) {
    if (self == NULL) {
        printf("arena: reset failed, invalid instance\n");
        return;
    }
    const unsigned int space = MEMORY_SPACE_STD(ArenaMemory);
    self->offset = self->padding + space;
}

void arena_destroy(ArenaMemory **self) {
    if (self == NULL || *self == NULL) {
        printf("arena: destroy failed, invalid instance\n");
        return;
    }
    size_t op = (size_t) (*self) - (*self)->padding;
    free((void *) (op));
    *self = NULL;
}

ArenaMemory *arena_create(void *m, size_t size) {
    size_t address = (size_t) m;
    const unsigned int space = MEMORY_SPACE_STD(ArenaMemory);
    const unsigned int padding = MEMORY_PADDING_STD(address);
    ArenaMemory *self = (ArenaMemory *) (address + padding);
    self->size = size;
    self->offset = padding + space;
    self->padding = padding;
    return self;
}

ArenaMemory *make_arena(size_t size) {
    void *m = malloc(size);
    if (m == NULL) {
        printf("arena: make failed, system can't provide free memory\n");
        exit(EXIT_FAILURE);
    }
    return arena_create(m, size);
}

ArenaMemory *make_arena_exact(size_t size) {
    size += MEMORY_SPACE_STD(ArenaMemory) + sizeof(size_t);
    return make_arena(size);
}
