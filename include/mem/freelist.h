#pragma once


#include <stddef.h>

typedef struct __attribute__((aligned(16), packed)) {
    void *next;
    unsigned int total;
    unsigned int padding;
} FreeListMemory;

FreeListMemory *make_freelist(unsigned int size);

FreeListMemory *freelist_create(void *m, unsigned int size);

void freelist_destroy(FreeListMemory **self);

void freelist_reset(FreeListMemory *self);

void *freelist_alloc(FreeListMemory *self, unsigned int size, unsigned int alignment);

char freelist_free(FreeListMemory *self, void **ptr);

unsigned int freelist_usage(FreeListMemory *self);


