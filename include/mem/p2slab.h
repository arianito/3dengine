#pragma once

#include <stddef.h>
#include "mem/utils.h"

#define P2SLAB_MAX 29

typedef struct __attribute__((aligned(512), packed)) {
    GeneralAllocator _allocator;
    char _pools[P2SLAB_MAX * 16];
    unsigned short _n;
    unsigned short _padding;
    unsigned int total;
    unsigned int usage;
} P2SlabMemory;

P2SlabMemory *p2slab_create(void *m, unsigned int n);

P2SlabMemory *p2slab_create_alloc(GeneralAllocator allocator, unsigned int n);

P2SlabMemory *make_p2slab(unsigned int n);

void p2slab_destroy(P2SlabMemory **self);

void p2slab_fit(P2SlabMemory *self);

void *p2slab_alloc(P2SlabMemory *self, unsigned int size);

char p2slab_free(P2SlabMemory *self, void **ptr);