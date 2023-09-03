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
    size_t used;
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
        node->used = 0;
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
    node->used = 1;
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

    if (!node->used)
    {
        if (err != NULL)
            *err = READY;
        return;
    }

    node->used = 0;
    node->next = instance->head;
    instance->head = node;
    instance->capacity++;

    if (err != NULL)
        *err = READY;

    (*p) = NULL;
}
/*

typedef struct
{
    void *next;
    unsigned char used;
} PoolNode;

typedef struct
{
    void *head;
    void *tail;
    size_t size;
    size_t chunk_size;
    size_t free;
    unsigned char alignment;
    unsigned char alignment_padding;
} PoolHeader;

void mpool_enqueue(PoolHeader *header, PoolNode *node)
{
    node->used = 0;
    node->next = NULL;
    if (header->head == NULL)
    {
        header->head = node;
        header->tail = node;
        return;
    }

    ((PoolNode *)header->tail)->next = node;
    header->tail = node;
}

void *mpool_dequeue(PoolHeader *header)
{
    if (header->head == NULL)
        return NULL;

    PoolNode *node = (PoolNode *)header->head;
    node->used = 1;
    header->head = node->next;

    if (header->head == NULL)
        header->tail = NULL;

    return node;
}

void *mpool_create(size_t size, size_t chunk_size, unsigned char alignment)
{
    assert((alignment & (alignment - 1)) == 0 && "invalid alignment");

    void *mem = malloc(size);
    assert(mem != NULL && "insufficient memory");
    size_t address = (size_t)mem;
    unsigned char header_size = sizeof(PoolHeader);
    unsigned char padding = calculate_padding(address, header_size, alignment);
    unsigned char alignment_padding = padding - header_size;

    PoolHeader *header = (PoolHeader *)(address + alignment_padding);
    header->head = NULL;
    header->tail = NULL;
    header->size = size;
    header->free = 0;
    header->chunk_size = chunk_size;
    header->alignment = alignment;
    header->alignment_padding = alignment_padding;

    unsigned char node_size = sizeof(PoolNode);
    size_t cursor = padding;
    while (1)
    {
        size_t chunk_address = address + cursor;
        unsigned char node_padding = calculate_padding(chunk_address, node_size, alignment);
        unsigned char alignment_padding = node_padding - node_size;

        cursor += node_padding + chunk_size;

        if (cursor > size)
            break;

        mpool_enqueue(header, (PoolNode *)(chunk_address + alignment_padding));
        header->free++;
    }

    return (void *)header;
}
void mpool_destroy(void *mem)
{
    unsigned char header_size = sizeof(PoolHeader);
    PoolHeader *header = (PoolHeader *)(mem);
    void *p = (void *)((size_t)header - header->alignment_padding);
    free(p);
    mem = NULL;
}

char mpool_check(void *mem)
{
    unsigned char header_size = sizeof(PoolHeader);
    PoolHeader *header = (PoolHeader *)(mem);
    return header->head != NULL;
}

void *mpool_alloc(void *mem)
{
    unsigned char header_size = sizeof(PoolHeader);
    PoolHeader *header = (PoolHeader *)(mem);

    assert(header->head != NULL && "not enough memory");

    unsigned char node_size = sizeof(PoolNode);
    PoolNode *node = mpool_dequeue(header);

    header->free--;
    return (void *)((size_t)node + node_size);
}

void mpool_free(void *mem, void *p)
{
    assert(p != NULL && "already freed");

    unsigned char header_size = sizeof(PoolHeader);
    PoolHeader *header = (PoolHeader *)(mem);

    size_t start_address = (size_t)header - header->alignment_padding;
    size_t address = (size_t)p;

    assert(address > start_address && address < (start_address + header->size) && "invalid ptr");

    unsigned char node_size = sizeof(PoolNode);
    PoolNode *node = (PoolNode *)((size_t)p - node_size);

    assert(node->used == 1 && "already freed");

    mpool_enqueue(header, node);

    header->free++;

    p = NULL;
}

size_t mpool_space(void *mem)
{
    unsigned char node_size = sizeof(PoolNode);
    unsigned char header_size = sizeof(PoolHeader);
    PoolHeader *header = (PoolHeader *)(mem);
    size_t size = header->size;
    size_t address = (size_t)header - header->alignment_padding;
    size_t cursor = header->alignment_padding + header_size;
    size_t space = 0;
    while (1)
    {
        size_t chunk_address = address + cursor;
        unsigned char node_padding = calculate_padding(chunk_address, node_size, header->alignment);
        unsigned char node_alignment_padding = node_padding - node_size;

        cursor += node_padding + header->chunk_size;

        if (cursor > size)
            break;

        PoolNode *node = (PoolNode *)(chunk_address + node_alignment_padding);

        if (node->used && cursor > space)
            space = cursor;
    }

    return space;
}
void *mpool_resize(void *mem, size_t new_size)
{
    unsigned char node_size = sizeof(PoolNode);
    unsigned char header_size = sizeof(PoolHeader);
    PoolHeader *header = (PoolHeader *)(mem);
    size_t alignment_padding = header->alignment_padding;
    size_t address = (size_t)header - alignment_padding;
    size_t cursor = alignment_padding + header_size;
    size_t space = mpool_space(mem);

    assert(new_size >= space && "new_size must be larger than current allocation space");

    void *new_mem = realloc((void *)address, new_size);
    assert(new_mem != NULL && "insufficient memory");

    header = (PoolHeader *)((size_t)new_mem + alignment_padding);
    address = (size_t)new_mem;
    header->head = NULL;
    header->tail = NULL;
    header->free = 0;

    while (1)
    {
        size_t chunk_address = address + cursor;
        unsigned char node_padding = calculate_padding(chunk_address, node_size, header->alignment);
        unsigned char node_alignment_padding = node_padding - node_size;

        cursor += node_padding + header->chunk_size;

        if (cursor > new_size)
            break;

        PoolNode *node = (PoolNode *)(chunk_address + node_alignment_padding);
        unsigned char used = cursor > space ? 0 : node->used;
        if (!used)
        {
            mpool_enqueue(header, node);
            header->free++;
        }
        node->used = used;
    }

    header->size = new_size;
    return header;
}

void mpool_usage(void *mem)
{
    unsigned char header_size = sizeof(PoolHeader);
    unsigned char node_size = sizeof(PoolNode);
    PoolHeader *header = (PoolHeader *)(mem);
    size_t address = (size_t)header - header->alignment_padding;
    size_t cursor = header->alignment_padding + header_size;
    unsigned char alignment = header->alignment;
    size_t chunk_size = header->chunk_size;

    while (1)
    {
        size_t chunk_address = address + cursor;
        unsigned char node_padding = calculate_padding(chunk_address, node_size, alignment);
        unsigned char alignment_padding = node_padding - node_size;

        if (cursor + node_padding + chunk_size > header->size)
            break;

        PoolNode *node = (PoolNode *)(chunk_address + alignment_padding);
        printf("[%zu-%zu]: %d\n", (size_t)cursor, (size_t)(cursor + node_padding + chunk_size), node->used);

        cursor += node_padding + chunk_size;
    }
    printf("free pools:%zu, size:%zu\n", header->free, header->size);
}

*/