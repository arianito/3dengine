#include "mem.h"

#include <malloc.h>
#include <stdio.h>
#include <assert.h>

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

ArenaMemory *arenap(void *m, size_t size, error_t *err)
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

ArenaMemory *arena(size_t size, error_t *err)
{
    void *m = malloc(size);
    if (m == NULL)
    {
        if (err != NULL)
            *err = INSUFFICIENT_MEMORY;
        return NULL;
    }
    return arenap(m, size, err);
}

void arena_destroy(ArenaMemory **arena, error_t *err)
{
    if (*arena == NULL)
    {
        if (err != NULL)
            *err = INVALID_INPUT;
        return;
    }
    size_t original_pointer = (size_t)(*arena) - (*arena)->padding;
    free((void *)(original_pointer));

    (*arena) = NULL;
    if (err != NULL)
        *err = READY;
}

void *arena_ptr(ArenaMemory *arena, error_t *err)
{
    if (arena == NULL)
    {
        if (err != NULL)
            *err = INVALID_INPUT;
        return NULL;
    }
    size_t original_pointer = (size_t)arena - arena->padding;
    return (void *)(original_pointer);
}

void *arena_allocate(ArenaMemory *arena, size_t size, int alignment, error_t *err)
{
    if (arena == NULL)
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

    size_t address = ((size_t)arena - arena->padding) + arena->offset;
    int padding = calculate_padding(address, alignment);

    if (arena->offset + size + padding > arena->size)
    {
        if (err != NULL)
            *err = ALLOCATION_FAILED;
        return NULL;
    }
    address += padding;
    arena->offset += size + padding;
    return (void *)(address);
}

void arena_reset(ArenaMemory *arena)
{
    int space = calculate_space(sizeof(ArenaMemory), sizeof(size_t));
    arena->offset = arena->padding + space;
}

// stack memory allocator

typedef struct
{
    void *next;
    size_t padding;
} stacknode_t;

StackMemory *stackp(void *m, size_t size, error_t *err)
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
StackMemory *stack(size_t size, error_t *err)
{

    void *m = malloc(size);
    if (m == NULL)
    {
        if (err != NULL)
            *err = INSUFFICIENT_MEMORY;
        return NULL;
    }
    return stackp(m, size, err);
}

void stack_destroy(StackMemory **stack, error_t *err)
{
    if (*stack == NULL)
    {
        if (err != NULL)
            *err = INVALID_INPUT;
        return;
    }
    size_t original_pointer = (size_t)(*stack) - (*stack)->padding;
    free((void *)(original_pointer));

    (*stack) = NULL;

    if (err != NULL)
        *err = READY;
}

void *stack_ptr(StackMemory **stack, error_t *err)
{
    if (*stack == NULL)
    {
        if (err != NULL)
            *err = INVALID_INPUT;
        return NULL;
    }
    size_t original_pointer = (size_t)(*stack) - (*stack)->padding;
    return ((void *)(original_pointer));
}

void *stack_allocate(StackMemory *stack, size_t size, int alignment, error_t *err)
{
    if (stack == NULL)
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

    size_t address = ((size_t)stack - stack->padding) + stack->offset;
    int padding = calculate_alignment(address, sizeof(stacknode_t), alignment);
    int space = calculate_space(sizeof(stacknode_t), sizeof(size_t));

    if (stack->offset + padding + size > stack->size)
    {
        if (err != NULL)
            *err = ALLOCATION_FAILED;
        return NULL;
    }

    address += padding;
    stack->offset += padding + size;

    if (stack->offset > stack->peak)
        stack->peak = stack->offset;

    stacknode_t *node = (stacknode_t *)(address - space);
    node->next = stack->head;
    node->padding = padding;

    stack->head = node;

    if (err != NULL)
        *err = READY;

    return (void *)address;
}

void stack_free(StackMemory *stack, void **p, error_t *err)
{

    if (stack == NULL || p == NULL || (*p) == NULL)
    {
        if (err != NULL)
            *err = INVALID_INPUT;
        return;
    }

    size_t start = (size_t)stack - stack->padding;
    size_t address = (size_t)(*p);
    size_t end = start + stack->size;

    if (!(address >= start || address < end))
    {
        if (err != NULL)
            *err = OUT_OF_BOUNDARY;
        return;
    }

    if (address >= start + stack->offset)
        return;

    int space = calculate_space(sizeof(stacknode_t), sizeof(size_t));
    stacknode_t *node = (stacknode_t *)(address - space);

    if (node != stack->head)
    {
        if (err != NULL)
            *err = OUT_OF_BOUNDARY;
        return;
    }

    stack->offset = (address - start) - node->padding;
    stack->head = ((stacknode_t *)stack->head)->next;

    (*p) = NULL;

    if (err != NULL)
        *err = READY;
}

void stack_reset(StackMemory *stack)
{
    int space = calculate_space(sizeof(ArenaMemory), sizeof(size_t));
    stack->offset = stack->padding + space;
    stack->peak = stack->offset;
    stack->head = NULL;
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