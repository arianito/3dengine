#pragma once

#include <stddef.h>
#include "mem/utils.h"

typedef struct {
    size_t next;
} SlabObject;

typedef struct {
    void *next;
    unsigned int size;
    unsigned int padding;
} SlabPage;

typedef struct {
    GeneralAllocator allocator;
    SlabObject *objects;
    SlabPage *pages;
    unsigned int padding;
    unsigned int slabSize;
    unsigned int objectSize;
    unsigned int usage;
    unsigned int total;
} SlabMemory;

SlabMemory *slab_create(void *m, unsigned int slabSize, unsigned short objectSize);

SlabMemory *slab_create_alloc(GeneralAllocator allocator, unsigned int slabSize, unsigned short objectSize);

SlabMemory *make_slab(unsigned int slabSize, unsigned short objectSize);

void slab_destroy(SlabMemory **self);

void *slab_alloc(SlabMemory *self);

char slab_free(SlabMemory *self, void **ptr);