#pragma once

#include <stddef.h>

typedef struct {
    void *(*alloc)(size_t);

    void (*free)(void *);
} P2SlabAllocator;

typedef struct {
    size_t next;
} P2SlabObject;

typedef struct {
    void *next;
    unsigned int size;
    unsigned int padding;
} P2SlabPage;

typedef struct {
    P2SlabObject *objects;
    P2SlabPage *pages;
} P2SlabPool;

typedef struct {
    P2SlabAllocator allocator;
    P2SlabPool pools[32];
    unsigned int padding;
    unsigned int usage;
    unsigned int total;
    unsigned int n;
} P2SlabMemory;

P2SlabMemory *p2slab_create(void *m, unsigned int n);

P2SlabMemory *p2slab_create_alloc(P2SlabAllocator allocator, unsigned int n);

P2SlabMemory *make_p2slab(unsigned int n);

void p2slab_destroy(P2SlabMemory **self);

void *p2slab_alloc(P2SlabMemory *self, unsigned int size);

char p2slab_free(P2SlabMemory *self, void **ptr);