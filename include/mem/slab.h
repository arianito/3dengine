#pragma once

#include <stddef.h>

typedef struct {
    size_t next;
} SlabObject;

typedef struct {
    void *next;
    unsigned int size;
    unsigned int padding;
} SlabPage;

typedef struct {
    SlabObject *objects;
    SlabPage *pages;
    unsigned int padding;
    unsigned int slabSize;
    unsigned int objectSize;
    unsigned int capacity;
    unsigned int usage;
    unsigned int bytes;

    void *(*allocator)(size_t);
} SlabMemory;

SlabMemory *slab_create(void *m, unsigned int slabSize, unsigned short objectSize);

SlabMemory *make_slab(unsigned int slabSize, unsigned short objectSize);

void slab_destroy(SlabMemory **self);

void *slab_alloc(SlabMemory *self);

char slab_free(SlabMemory *self, void **ptr);