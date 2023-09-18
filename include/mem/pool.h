#pragma once


#include <stddef.h>

typedef struct {
    size_t next; // 7bytes offset 1byte used
} PoolMemoryNode;

typedef struct {
    PoolMemoryNode *head;
    unsigned int size;
    unsigned short padding;
    unsigned short capacity;
} PoolMemory;

void *pool_alloc(PoolMemory *self);

unsigned char pool_free(PoolMemory *self, void **ptr);

void pool_destroy(PoolMemory **self);

PoolMemory *pool_create(void *m, size_t size, unsigned int chunkSize);

PoolMemory *make_pool(size_t size, unsigned int chunkSize);

PoolMemory *make_pool_exact(size_t size, unsigned int chunkSize);