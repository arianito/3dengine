
#include "mem/pool.h"

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

#include "mem/utils.h"

void pool_enqueue(PoolMemory *self, PoolMemoryNode *node) {
    node->next = BYTE71((size_t) self->_head, 0);
    self->_head = node;
}

PoolMemoryNode *pool_dequeue(PoolMemory *self) {
    if (self->_head == NULL)
        return NULL;

    PoolMemoryNode *node = self->_head;
    node->next = BYTE71_SET_1(node->next, 1);
    self->_head = (PoolMemoryNode *) (BYTE71_GET_7(node->next));
    return node;
}

void *pool_alloc(PoolMemory *self) {
#if MEM_DEBUG_MODE
    if (self == NULL) {
        printf("pool: alloc failed, invalid instance\n");
        return NULL;
    }
    if (self->_head == NULL) {
        printf("pool: alloc failed, insufficient memory\n");
        return NULL;
    }
#endif
    PoolMemoryNode *node = pool_dequeue(self);
    self->usage -= self->_objectSize;
    const unsigned int space = MEMORY_SPACE_STD(PoolMemoryNode);
    return (void *) ((size_t) node + space);
}

unsigned char pool_free(PoolMemory *self, void **p) {
#if MEM_DEBUG_MODE
    if (self == NULL) {
        printf("pool: free failed, invalid instance\n");
        return 0;
    }
    if (p == NULL || (*p) == NULL) {
        printf("pool: free failed, invalid pointer\n");
        return 0;
    }
#endif
    size_t address = (size_t) (*p);

    const unsigned int space = MEMORY_SPACE_STD(PoolMemoryNode);
    PoolMemoryNode *node = (PoolMemoryNode *) (address - space);

    unsigned char used = BYTE71_GET_1(node->next);
    if (!used) {
        printf("pool: free failed, already freed\n");
        return 0;
    }

    pool_enqueue(self, node);
    self->usage += self->_objectSize;
    (*p) = NULL;
    return 1;
}

void pool_destroy(PoolMemory **self) {
    if (self == NULL || *self == NULL) {
        printf("pool: destroy failed, invalid instance\n");
        return;
    }
    size_t op = (size_t) (*self) - (*self)->_padding;
    free((void *) (op));
    (*self) = NULL;
}

PoolMemory *pool_create(void *m, unsigned int size, unsigned int objectSize) {
    size_t start = (size_t) m;
    unsigned int space = MEMORY_SPACE_STD(PoolMemory);
    unsigned int padding = MEMORY_PADDING_STD(start);
    PoolMemory *self = (PoolMemory *) (start + padding);
    self->_head = NULL;
    self->total = size;
    self->_padding = padding;
    self->_objectSize = objectSize;
    self->usage = 0;
    size_t cursor = padding + space;
    space = MEMORY_SPACE_STD(PoolMemoryNode);
    while (1) {
        size_t address = start + cursor;
        padding = MEMORY_ALIGNMENT_STD(address, PoolMemoryNode);
        cursor += padding + objectSize;
        if (cursor > size)
            break;

        pool_enqueue(self, (PoolMemoryNode *) (address + padding - space));
        self->usage += self->_objectSize;
    }
    return self;
}

PoolMemory *make_pool(unsigned int size, unsigned int objectSize) {
    void *m = malloc(size);
    if (m == NULL) {
        printf("pool: make failed, system can't provide free memory\n");
        exit(EXIT_FAILURE);
    }
    return pool_create(m, size, objectSize);
}

unsigned int pool_size(unsigned int size, unsigned int objectSize) {
    unsigned int n = size / objectSize;
    size += MEMORY_SPACE_STD(PoolMemory) + sizeof(size_t);
    size += n * sizeof(PoolMemoryNode);
    return size;
}