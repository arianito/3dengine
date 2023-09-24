#pragma once

#include <stddef.h>
#include "mem/utils.h"

typedef struct __attribute__((aligned(64), packed)) {
    GeneralAllocator _allocator;
    void *_objects;
    void *_pages;
    unsigned int _slabSize;
    unsigned int _objectSize;
    unsigned int _padding;
    unsigned int total;
    unsigned int usage;
} SlabMemory;

SlabMemory *slab_create(void *m, unsigned int slabSize, unsigned short objectSize);

SlabMemory *slab_create_alloc(GeneralAllocator allocator, unsigned int slabSize, unsigned short objectSize);

SlabMemory *make_slab(unsigned int slabSize, unsigned short objectSize);

void slab_destroy(SlabMemory **self);

void slab_fit(SlabMemory *self);

void *slab_alloc(SlabMemory *self);

char slab_free(SlabMemory *self, void **ptr);