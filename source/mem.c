#include "mem.h"

#include <malloc.h>
#include <stdio.h>
#include <assert.h>

unsigned char calculate_padding(size_t address, unsigned char space, unsigned char alignment)
{
    unsigned char padding = 0;
    unsigned char modulo = address & (alignment - 1);
    
    if (modulo != 0)
        padding = alignment - modulo;

    if (space > 0 && space > padding)
        padding += alignment * ((space / alignment) + ((space & (alignment - 1)) > 0 ? 1 : 0));

    return padding;
}

// stack memory allocator

typedef struct
{
    void *next;
    unsigned char padding;
} StackNode;

typedef struct
{
    void *head;
    size_t size;
    size_t seek;
    size_t peak;
    unsigned char alignment_padding;
} StackHeader;

void *mstack_create(size_t size, unsigned char alignment)
{
    assert((alignment & (alignment - 1)) == 0 && "invalid alignment");

    void *mem = malloc(size);
    assert(mem != NULL && "insufficient memory");
    size_t address = (size_t)mem;
    unsigned char header_size = sizeof(StackHeader);
    unsigned char padding = calculate_padding(address, header_size, alignment);
    unsigned char alignment_padding = padding - header_size;

    StackHeader *header = (StackHeader *)(address + alignment_padding);
    header->head = NULL;
    header->size = size;
    header->seek = padding;
    header->peak = header->seek;
    header->alignment_padding = alignment_padding;

    return (void *)header;
}

void mstack_destroy(void *mem)
{
    unsigned char header_size = sizeof(StackHeader);
    StackHeader *header = (StackHeader *)(mem);
    void *p = (void *)((size_t)header - header->alignment_padding);
    free(p);
    mem = NULL;
}

char mstack_check(void *mem, size_t size, unsigned char alignment)
{
    if ((alignment & (alignment - 1)) != 0)
        return 0;

    unsigned char node_size = sizeof(StackNode);
    StackHeader *header = (StackHeader *)(mem);
    size_t address = (size_t)mem - header->alignment_padding + header->seek;
    unsigned char padding = calculate_padding(address, node_size, alignment);

    if (header->seek + padding + size > header->size)
        return 0;

    return 1;
}

void *mstack_alloc(void *mem, size_t size, unsigned char alignment)
{
    assert((alignment & (alignment - 1)) == 0 && "invalid alignment");

    unsigned char node_size = sizeof(StackNode);
    StackHeader *header = (StackHeader *)(mem);
    size_t address = (size_t)mem - header->alignment_padding + header->seek;
    unsigned char padding = calculate_padding(address, node_size, alignment);

    assert(header->seek + padding + size <= header->size && "not enough memory");

    address += padding;
    header->seek += padding + size;

    if (header->seek > header->peak)
        header->peak = header->seek;

    StackNode *node = (StackNode *)(address - node_size);
    node->next = header->head;
    node->padding = padding;
    header->head = node;

    return (void *)address;
}

void mstack_free(void *mem, void *p)
{
    assert(p != NULL && "already freed");

    unsigned char node_size = sizeof(StackNode);
    StackHeader *header = (StackHeader *)(mem);
    StackNode *node = (StackNode *)((size_t)p - node_size);

    assert(node == header->head && "invalid ptr");

    header->seek = ((size_t)p - node->padding) - ((size_t)header - header->alignment_padding);
    header->head = ((StackNode *)header->head)->next;
    p = NULL;
}

void *mstack_resize(void *mem, size_t new_size)
{
    unsigned char header_size = sizeof(StackHeader);
    StackHeader *header = (StackHeader *)(mem);
    unsigned char alignment_padding = header->alignment_padding;

    assert(new_size != header->size && "new size must differ");

    assert(new_size > header->seek && "new_size must be larger than current allocation space");

    void *p = (void *)((size_t)header - header->alignment_padding);

    void *new_mem = realloc(p, new_size);

    assert(new_mem != NULL && "insufficient memory");

    header = (StackHeader *)((size_t)new_mem + alignment_padding);

    header->size = new_size;
    header->peak = header->seek;

    return header;
}

void mstack_usage(void *mem)
{
    StackHeader *header = (StackHeader *)(mem);
    printf("space:%zu/%zu, peak:%zu\n", header->seek, header->size, header->peak);
}

// pool memory allocator

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

        mpool_enqueue(header, (PoolNode*)(chunk_address + alignment_padding));
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