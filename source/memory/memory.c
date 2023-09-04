#include "memory/memory.h"

#include <malloc.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

void format_bytes(double bytes, char *buff, size_t n)
{
    const char suffix[5][3] = {"B\0", "KB\0", "MB\0", "GB\0", "TB\0"};
    int i = 0;

    while (bytes >= 1000 && i < 5)
    {
        bytes /= 1000;
        i++;
    }

    snprintf(buff, n, "%.2f %s", bytes, suffix[i]);
}

inline int calculate_padding(size_t address, int alignment)
{
    int modulo = address & (alignment - 1);

    if (modulo == 0)
        return 0;

    return alignment - modulo;
}

inline int calculate_space(int space, int alignment)
{
    int padding = alignment * (space / alignment);
    int modulo = space & (alignment - 1);

    if (modulo == 0)
        return padding;

    return padding + alignment;
}

inline int calculate_alignment(size_t address, int space, int alignment)
{
    int padding = calculate_padding(address, alignment);
    if (padding >= space)
        return padding;
    return padding + calculate_space(space, alignment);
}

void clear(void *p, size_t s)
{
    memset(p, 0, s);
}

void *std_alloc(size_t size, int alignment)
{
    assert((alignment & (alignment - 1)) == 0 && "invalid alignment");
    void *ptr = malloc(size + alignment);
    if (ptr == NULL)
    {
        printf("std: system can't provide free memory\n");
        exit(EXIT_FAILURE);
        return NULL;
    }

    size_t address = (size_t)ptr;
    int padding = calculate_alignment(address, 1, alignment);
    address += padding;
    *((char *)(address - 1)) = padding;
    return (void *)address;
}

void std_free(void **ptr)
{
    if (ptr == NULL || *ptr == NULL)
        return;
    size_t address = (size_t)(*ptr);
    free((void*)(address - *((char *)(address - 1))));
}

void *arena_alloc(ArenaMemory *self, size_t size, int alignment)
{
    assert((alignment & (alignment - 1)) == 0 && "invalid alignment");
    if (self == NULL)
    {
        printf("arena: invalid instance\n");
        exit(EXIT_FAILURE);
        return NULL;
    }
    ArenaMemory *instance = self;
    size_t address = ((size_t)instance - instance->padding) + instance->offset;
    int padding = calculate_padding(address, alignment);
    if (instance->offset + size + padding > instance->size)
    {
        printf("arena: insufficient memory\n");
        exit(EXIT_FAILURE);
        return NULL;
    }
    address += padding;
    instance->offset += size + padding;
    return (void *)(address);
}

void arena_reset(ArenaMemory *self)
{
    if (self == NULL)
        return;
    ArenaMemory *instance = self;
    int space = calculate_space(sizeof(ArenaMemory), sizeof(size_t));
    instance->offset = instance->padding + space;
}

void arena_destroy(ArenaMemory **self)
{
    if (self == NULL || *self == NULL)
        return;
    ArenaMemory *instance = *self;
    size_t op = (size_t)instance - instance->padding;
    free((void *)(op));
    *self = NULL;
}

ArenaMemory *make_arena_from(void *m, size_t size)
{
    size_t address = (size_t)m;
    int space = calculate_space(sizeof(ArenaMemory), sizeof(size_t));
    int padding = calculate_padding(address, sizeof(size_t));
    ArenaMemory *instance = (ArenaMemory *)(address + padding);
    instance->size = size;
    instance->offset = padding + space;
    instance->padding = padding;
    return instance;
}

ArenaMemory *make_arena(size_t size)
{
    void *m = malloc(size);
    if (m == NULL)
    {
        printf("arena: system can't provide free memory\n");
        exit(EXIT_FAILURE);
        return NULL;
    }
    return make_arena_from(m, size);
}

// stack memory allocator

typedef struct
{
    void *next;
    size_t padding;
} StackMemoryNode;

void *stack_alloc(StackMemory *self, size_t size, int alignment)
{
    assert((alignment & (alignment - 1)) == 0 && "invalid alignment");
    if (self == NULL)
    {
        printf("stack: invalid instance\n");
        exit(EXIT_FAILURE);
        return NULL;
    }
    StackMemory *instance = self;
    size_t address = ((size_t)instance - instance->padding) + instance->offset;
    int padding = calculate_alignment(address, sizeof(StackMemoryNode), alignment);
    int space = calculate_space(sizeof(StackMemoryNode), sizeof(size_t));
    if (instance->offset + padding + size > instance->size)
    {
        printf("stack: insufficient memory\n");
        exit(EXIT_FAILURE);
        return NULL;
    }
    address += padding;
    instance->offset += padding + size;
    if (instance->offset > instance->peak)
        instance->peak = instance->offset;
    StackMemoryNode *node = (StackMemoryNode *)(address - space);
    node->next = instance->head;
    node->padding = padding;
    instance->head = node;
    return (void *)address;
}

void stack_free(StackMemory *self, void **p)
{
    if (self == NULL || p == NULL || (*p) == NULL)
        return;
    StackMemory *instance = self;
    size_t start = (size_t)instance - instance->padding;
    size_t address = (size_t)(*p);
    size_t end = start + instance->size;
    if (!(address >= start || address < end))
        return;
    if (address >= start + instance->offset)
        return;
    int space = calculate_space(sizeof(StackMemoryNode), sizeof(size_t));
    StackMemoryNode *node = (StackMemoryNode *)(address - space);
    if (node != instance->head)
        return;
    instance->offset = (address - start) - node->padding;
    instance->head = ((StackMemoryNode *)instance->head)->next;
    *p = NULL;
}

void stack_reset(StackMemory *self)
{
    if (self == NULL)
        return;
    StackMemory *instance = self;
    int space = calculate_space(sizeof(ArenaMemory), sizeof(size_t));
    instance->offset = instance->padding + space;
    instance->peak = instance->offset;
    instance->head = NULL;
}

void stack_destroy(StackMemory **self)
{
    if (self == NULL || *self == NULL)
        return;
    StackMemory *instance = *self;
    size_t op = (size_t)instance - instance->padding;
    free((void *)(op));
    *self = NULL;
}

StackMemory *make_stack_from(void *m, size_t size)
{
    size_t address = (size_t)m;
    int space = calculate_space(sizeof(StackMemory), sizeof(size_t));
    int padding = calculate_padding(address, sizeof(size_t));
    StackMemory *instance = (StackMemory *)(address + padding);
    instance->head = NULL;
    instance->size = size;
    instance->offset = padding + space;
    instance->peak = instance->offset;
    instance->padding = padding;
    return instance;
}
StackMemory *make_stack(size_t size)
{
    void *m = malloc(size);
    if (m == NULL)
    {
        printf("stack: system can't provide free memory\n");
        exit(EXIT_FAILURE);
        return NULL;
    }
    return make_stack_from(m, size);
}

// pool

typedef struct
{
    void *next;
} PoolMemoryNode;

void *pool_alloc(PoolMemory *self)
{
    if (self == NULL)
    {
        printf("pool: invalid instance\n");
        exit(EXIT_FAILURE);
        return NULL;
    }
    PoolMemory *instance = self;
    if (instance->head == NULL)
    {
        printf("pool: insufficient memory\n");
        exit(EXIT_FAILURE);
        return NULL;
    }

    PoolMemoryNode *node = instance->head;
    instance->head = node->next;
    instance->capacity--;
    int space = calculate_space(sizeof(PoolMemoryNode), sizeof(size_t));
    return (void *)((size_t)node + space);
}

void pool_free(PoolMemory *self, void **p)
{
    if (self == NULL || p == NULL || (*p) == NULL)
        return;
    PoolMemory *instance = self;
    size_t start = (size_t)instance - instance->padding;
    size_t address = (size_t)(*p);
    size_t end = start + instance->size;

    if (!(address >= start || address < end))
        return;
    int space = calculate_space(sizeof(PoolMemoryNode), sizeof(size_t));
    PoolMemoryNode *node = (PoolMemoryNode *)(address - space);
    node->next = instance->head;
    instance->head = node;
    instance->capacity++;
    (*p) = NULL;
}

void pool_destroy(PoolMemory **self)
{
    if (self == NULL || *self == NULL)
        return;
    PoolMemory *instance = *self;
    size_t op = (size_t)instance - instance->padding;
    free((void *)(op));
    (*self) = NULL;
}

PoolMemory *make_pool_from(void *m, size_t size, size_t chunkSize)
{
    size_t address = (size_t)m;
    int space = calculate_space(sizeof(StackMemory), sizeof(size_t));
    int padding = calculate_padding(address, sizeof(size_t));
    PoolMemory *instance = (PoolMemory *)(address + padding);
    instance->head = NULL;
    instance->size = size;
    instance->padding = padding;
    instance->capacity = 0;
    size_t cursor = padding + space;
    while (1)
    {
        size_t chunk_address = address + cursor;
        padding = calculate_alignment(chunk_address, sizeof(PoolMemoryNode), sizeof(size_t));
        space = calculate_space(sizeof(PoolMemoryNode), sizeof(size_t));
        if (cursor + padding + chunkSize > size)
            break;
        cursor += padding + chunkSize;
        chunk_address += padding;
        PoolMemoryNode *node = (PoolMemoryNode *)(chunk_address - space);
        node->next = instance->head;
        instance->head = node;
        instance->capacity++;
    }

    return instance;
}

PoolMemory *make_pool(size_t size, size_t chunkSize)
{
    void *m = malloc(size);
    if (m == NULL)
    {
        printf("pool: system can't provide free memory\n");
        exit(EXIT_FAILURE);
        return NULL;
    }
    return make_pool_from(m, size, chunkSize);
}
