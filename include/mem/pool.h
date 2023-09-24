#pragma once

#include <stddef.h>

typedef struct __attribute__((aligned(8), packed)) {
    size_t next; // 7bytes offset 1byte used
} PoolMemoryNode;

typedef struct __attribute__((aligned(32), packed)) {
    PoolMemoryNode *_head;
    unsigned int _padding;
    unsigned int _objectSize;
    unsigned int total;
    unsigned int usage;
} PoolMemory;

PoolMemory *make_pool(unsigned int size, unsigned int objectSize);

PoolMemory *pool_create(void *m, unsigned int size, unsigned int objectSize);

unsigned int pool_size(unsigned int size, unsigned int objectSize);

void pool_destroy(PoolMemory **self);

void *pool_alloc(PoolMemory *self);

unsigned char pool_free(PoolMemory *self, void **ptr);
