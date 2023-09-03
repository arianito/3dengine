#include "mem.h"

#include <malloc.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void clear(void *ptr, size_t s)
{
    memset(ptr, 0, s);
}

void format_bytes(double bytes, char *buff, size_t n)
{
    const char suffix[5][3] = {"B\0", "KB\0", "MB\0", "GB\0", "TB\0"};
    int i = 0;

    while (bytes >= 1024 && i < sizeof(suffix) / sizeof(suffix[0]) - 1)
    {
        bytes /= 1024;
        i++;
    }

    snprintf(buff, n, "%.2f %s", bytes, suffix[i]);
}

void handler_memory_error(MemoryErrorEnum err)
{
    if (err == READY)
        return;

    printf("memory operation failure: %d\n", err);
    switch (err)
    {
    case INVALID_INPUT:
        printf("INVALID_INPUT\n");
        break;
    case INSUFFICIENT_MEMORY:
        printf("INSUFFICIENT_MEMORY\n");
        break;
    case INVALID_ALIGNMENT:
        printf("INVALID_ALIGNMENT\n");
        break;
    case OUT_OF_BOUNDARY:
        printf("OUT_OF_BOUNDARY\n");
        break;
    default:
        break;
    }

    assert(0);
    exit(EXIT_FAILURE);
}

inline int calculate_padding(size_t address, int alignment)
{
    int modulo = address & (alignment - 1);

    if (modulo == 0)
        return 0;

    return alignment - modulo;
}

inline int calculate_space(int min_required_space, int alignment)
{
    int padding = alignment * (min_required_space / alignment);
    int modulo = min_required_space & (alignment - 1);

    if (modulo == 0)
        return padding;

    return padding + alignment;
}

inline int calculate_alignment(size_t address, int min_required_space, int alignment)
{
    int padding = calculate_padding(address, alignment);
    if (padding >= min_required_space)
        return padding;
    return padding + calculate_space(min_required_space, alignment);
}

ArenaMemory *arenap(void *m, size_t size, MemoryErrorEnum *err)
{
    size_t address = (size_t)m;

    int space = calculate_space(sizeof(ArenaMemory), sizeof(size_t));
    int padding = calculate_padding(address, sizeof(size_t));
    ArenaMemory *instance = (ArenaMemory *)(address + padding);
    instance->size = size;
    instance->offset = padding + space;
    instance->padding = padding;

    if (err != NULL)
        *err = READY;

    return instance;
}

ArenaMemory *arena(size_t size, MemoryErrorEnum *err)
{
    void *m = malloc(size);
    if (m == NULL)
    {
        if (err != NULL)
            *err = INSUFFICIENT_MEMORY;
        return NULL;
    }

    if (err != NULL)
        *err = READY;
    return arenap(m, size, err);
}

void arena_destroy(ArenaMemory **instance, MemoryErrorEnum *err)
{
    if (instance == NULL || *instance == NULL)
    {
        if (err != NULL)
            *err = INVALID_INPUT;
        return;
    }
    size_t original_pointer = (size_t)(*instance) - (*instance)->padding;
    free((void *)(original_pointer));

    (*instance) = NULL;

    if (err != NULL)
        *err = READY;
}

void *arena_allocate(ArenaMemory *instance, size_t size, int alignment, MemoryErrorEnum *err)
{
    if (instance == NULL)
    {
        if (err != NULL)
            *err = INVALID_INPUT;
        return NULL;
    }

    if ((alignment & (alignment - 1)) != 0)
    {
        if (err != NULL)
            *err = INVALID_ALIGNMENT;
        return NULL;
    }

    size_t address = ((size_t)instance - instance->padding) + instance->offset;
    int padding = calculate_padding(address, alignment);

    if (instance->offset + size + padding > instance->size)
    {
        if (err != NULL)
            *err = INSUFFICIENT_MEMORY;
        return NULL;
    }
    address += padding;
    instance->offset += size + padding;

    if (err != NULL)
        *err = READY;

    return (void *)(address);
}

void arena_reset(ArenaMemory *instance)
{
    int space = calculate_space(sizeof(ArenaMemory), sizeof(size_t));
    instance->offset = instance->padding + space;
}

// stack memory allocator

typedef struct
{
    void *next;
    size_t padding;
} StackMemoryNode;

StackMemory *stackp(void *m, size_t size, MemoryErrorEnum *err)
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

    if (err != NULL)
        *err = READY;

    return instance;
}
StackMemory *stack(size_t size, MemoryErrorEnum *err)
{
    void *m = malloc(size);
    if (m == NULL)
    {
        if (err != NULL)
            *err = INSUFFICIENT_MEMORY;
        return NULL;
    }

    if (err != NULL)
        *err = READY;
    return stackp(m, size, err);
}

void stack_destroy(StackMemory **instance, MemoryErrorEnum *err)
{
    if (instance == NULL || *instance == NULL)
    {
        if (err != NULL)
            *err = INVALID_INPUT;
        return;
    }
    size_t original_pointer = (size_t)(*instance) - (*instance)->padding;
    free((void *)(original_pointer));

    (*instance) = NULL;

    if (err != NULL)
        *err = READY;
}

void *stack_allocate(StackMemory *instance, size_t size, int alignment, MemoryErrorEnum *err)
{
    if (instance == NULL)
    {
        if (err != NULL)
            *err = INVALID_INPUT;
        return NULL;
    }

    if ((alignment & (alignment - 1)) != 0)
    {
        if (err != NULL)
            *err = INVALID_ALIGNMENT;
        return NULL;
    }

    size_t address = ((size_t)instance - instance->padding) + instance->offset;
    int padding = calculate_alignment(address, sizeof(StackMemoryNode), alignment);
    int space = calculate_space(sizeof(StackMemoryNode), sizeof(size_t));

    if (instance->offset + padding + size > instance->size)
    {
        if (err != NULL)
            *err = INSUFFICIENT_MEMORY;
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

    if (err != NULL)
        *err = READY;

    return (void *)address;
}

void stack_free(StackMemory *instance, void **p, MemoryErrorEnum *err)
{

    if (instance == NULL || p == NULL || (*p) == NULL)
    {
        if (err != NULL)
            *err = INVALID_INPUT;
        return;
    }

    size_t start = (size_t)instance - instance->padding;
    size_t address = (size_t)(*p);
    size_t end = start + instance->size;

    if (!(address >= start || address < end))
    {
        if (err != NULL)
            *err = OUT_OF_BOUNDARY;
        return;
    }

    if (address >= start + instance->offset)
        return;

    int space = calculate_space(sizeof(StackMemoryNode), sizeof(size_t));
    StackMemoryNode *node = (StackMemoryNode *)(address - space);

    if (node != instance->head)
    {
        if (err != NULL)
            *err = OUT_OF_BOUNDARY;
        return;
    }

    instance->offset = (address - start) - node->padding;
    instance->head = ((StackMemoryNode *)instance->head)->next;

    (*p) = NULL;

    if (err != NULL)
        *err = READY;
}

void stack_reset(StackMemory *instance)
{
    int space = calculate_space(sizeof(ArenaMemory), sizeof(size_t));
    instance->offset = instance->padding + space;
    instance->peak = instance->offset;
    instance->head = NULL;
}

// pool

typedef struct
{
    void *next;
} PoolMemoryNode;

PoolMemory *poolp(void *m, size_t size, size_t chunkSize, MemoryErrorEnum *err)
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

    if (err != NULL)
        *err = READY;

    return instance;
}

PoolMemory *pool(size_t size, size_t chunkSize, MemoryErrorEnum *err)
{
    void *m = malloc(size);
    if (m == NULL)
    {
        if (err != NULL)
            *err = INSUFFICIENT_MEMORY;
        return NULL;
    }

    if (err != NULL)
        *err = READY;

    return poolp(m, size, chunkSize, err);
}

void pool_destroy(PoolMemory **instance, MemoryErrorEnum *err)
{
    if (instance == NULL || *instance == NULL)
    {
        if (err != NULL)
            *err = INVALID_INPUT;
        return;
    }
    size_t original_pointer = (size_t)(*instance) - (*instance)->padding;
    free((void *)(original_pointer));

    (*instance) = NULL;

    if (err != NULL)
        *err = READY;
}

void *pool_allocate(PoolMemory *instance, MemoryErrorEnum *err)
{
    if (instance->head == NULL)
    {
        if (err != NULL)
            *err = INSUFFICIENT_MEMORY;
        return NULL;
    }

    PoolMemoryNode *node = instance->head;
    instance->head = node->next;

    instance->capacity--;

    int space = calculate_space(sizeof(PoolMemoryNode), sizeof(size_t));
    return (void *)((size_t)node + space);
}

void pool_free(PoolMemory *instance, void **p, MemoryErrorEnum *err)
{
    if (instance == NULL || p == NULL || (*p) == NULL)
    {
        if (err != NULL)
            *err = INVALID_INPUT;
        return;
    }

    size_t start = (size_t)instance - instance->padding;
    size_t address = (size_t)(*p);
    size_t end = start + instance->size;

    if (!(address >= start || address < end))
    {
        if (err != NULL)
            *err = OUT_OF_BOUNDARY;
        return;
    }

    int space = calculate_space(sizeof(PoolMemoryNode), sizeof(size_t));
    PoolMemoryNode *node = (PoolMemoryNode *)(address - space);

    node->next = instance->head;
    instance->head = node;
    instance->capacity++;

    if (err != NULL)
        *err = READY;

    (*p) = NULL;
}