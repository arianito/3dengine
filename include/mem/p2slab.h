#pragma once

#include <stddef.h>
#include "mem/utils.h"

#define P2SLAB_MAX 32

typedef struct __attribute__((aligned(512), packed)) {
    GeneralAllocator allocator;
    char pools[P2SLAB_MAX * 16];
    unsigned int total;
    unsigned int usage;
    unsigned short n;
    unsigned short padding;
} P2SlabMemory;

P2SlabMemory *p2slab_create(void *m, unsigned int n);

P2SlabMemory *p2slab_create_alloc(GeneralAllocator allocator, unsigned int n);

P2SlabMemory *make_p2slab(unsigned int n);

void p2slab_destroy(P2SlabMemory **self);

void *p2slab_alloc(P2SlabMemory *self, unsigned int size);

char p2slab_free(P2SlabMemory *self, void **ptr);