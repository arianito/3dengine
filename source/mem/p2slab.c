#include "mem/p2slab.h"

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

#include "mem/utils.h"


typedef struct __attribute__((aligned(16), packed)) {
    void *objects;
    void *pages;
} P2SlabPool;

typedef struct __attribute__((aligned(8), packed)) {
    size_t next;
} P2SlabObject;

typedef struct __attribute__((aligned(16), packed)) {
    void *next;
    unsigned int size;
    unsigned int padding;
} P2SlabPage;


#define P2SLAB_POOL(pools) ((P2SlabPool *) (&pools))

void p2slab_enqueue(P2SlabPool *pool, P2SlabObject *node, unsigned char order) {
    node->next = BYTE6AB((size_t) pool->objects, 0, order);
    pool->objects = node;
}

P2SlabObject *p2slab_dequeue(P2SlabPool *pool) {
    if (pool->objects == NULL)
        return NULL;

    P2SlabObject *node = pool->objects;
    node->next = BYTE6AB_SET_A(node->next, 1);
    pool->objects = (P2SlabObject *) (BYTE6AB_GET_6(node->next));
    return node;
}

P2SlabPage *create_p2slab(P2SlabMemory *self, unsigned int order) {
    unsigned int objectSize = 1 << order;

    unsigned int size = self->_n * objectSize;
    size += MEMORY_SPACE_STD(P2SlabPage);
    size += self->_n * MEMORY_SPACE_STD(P2SlabObject);
    size += sizeof(size_t);

    self->total += size;

    void *m = NULL;
    if (self->_allocator.alloc != NULL)
        m = self->_allocator.alloc(size);
    else
        m = malloc(size);
    if (m == NULL) {
        printf("p2slab create slab failed, system can't provide free memory\n");
        return NULL;
    }
    const size_t start = (size_t) m;
    unsigned int space = MEMORY_SPACE_STD(P2SlabPage);
    unsigned int padding = MEMORY_PADDING_STD(start);

    P2SlabPage *slab = (P2SlabPage *) (start + padding);
    slab->next = NULL;
    slab->size = size;
    slab->padding = padding;

    size_t cursor = padding + space;
    space = MEMORY_SPACE_STD(P2SlabObject);
    while (1) {
        size_t address = start + cursor;
        padding = MEMORY_ALIGNMENT_STD(address, P2SlabObject);
        cursor += padding + objectSize;
        if (cursor > size)
            break;

        P2SlabPool *pools = P2SLAB_POOL(self->_pools);
        p2slab_enqueue(&pools[order], (P2SlabObject *) (address + padding - space), order);
    }
    return slab;
}

P2SlabMemory *p2slab_create(void *m, unsigned int n) {
    const size_t start = (size_t) m;
    const unsigned int padding = MEMORY_PADDING_STD(start);

    P2SlabMemory *self = (P2SlabMemory *) (start + padding);
    self->_allocator.alloc = NULL;
    self->_allocator.free = NULL;
    self->_n = n;
    self->_padding = padding;
    self->usage = 0;
    self->total = padding + sizeof(P2SlabMemory);

    for (int i = 0; i < P2SLAB_MAX; i++) {
        P2SlabPool *pools = P2SLAB_POOL(self->_pools);
        pools[i].objects = NULL;
        pools[i].pages = NULL;
    }
    return self;
}

P2SlabMemory *p2slab_create_alloc(GeneralAllocator allocator, unsigned int n) {
    void *m = allocator.alloc(sizeof(P2SlabMemory));
    if (m == NULL) {
        printf("p2slab make failed, system can't provide free memory\n");
        exit(EXIT_FAILURE);
    }
    P2SlabMemory *slab = p2slab_create(m, n);
    slab->_allocator = allocator;
    return slab;
}

P2SlabMemory *make_p2slab(unsigned int n) {
    void *m = malloc(sizeof(P2SlabMemory));
    if (m == NULL) {
        printf("p2slab make failed, system can't provide free memory\n");
        exit(EXIT_FAILURE);
    }
    return p2slab_create(m, n);
}

void destroy_p2slab(P2SlabMemory *self, P2SlabPage *slab) {
    size_t op = (size_t) slab - slab->padding;
    if (self->_allocator.free != NULL)
        self->_allocator.free((void *) (op));
    else
        free((void *) (op));
}

void p2slab_destroy(P2SlabMemory **self) {
    if (self == NULL || *self == NULL) {
        printf("p2slab destroy failed, invalid instance\n");
        return;
    }

    for (int i = 0; i < P2SLAB_MAX; i++) {
        P2SlabPool *pools = P2SLAB_POOL((*self)->_pools);
        P2SlabPage *slab = pools[i].pages;
        while (slab != NULL) {
            void *next = slab->next;
            destroy_p2slab(*self, slab);
            slab = next;
        }
    }

    size_t op = (size_t) (*self) - (*self)->_padding;

    if ((*self)->_allocator.free != NULL)
        (*self)->_allocator.free((void *) (op));
    else
        free((void *) (op));
    (*self) = NULL;
}


int p2slab_next(int num) {
    if (num <= 0)
        return 0;

    num |= (num >> 1);
    num |= (num >> 2);
    num |= (num >> 4);
    num |= (num >> 8);
    num |= (num >> 16);

    return num + 1;
}

void *p2slab_alloc(P2SlabMemory *self, unsigned int size) {
#if MEM_DEBUG_MODE
    if (self == NULL) {
        printf("p2slab alloc failed, invalid instance\n");
        return NULL;
    }
#endif
    unsigned int order = 0;
    size = p2slab_next((int) size);
    while (size > 1) {
        size >>= 1;
        order++;
    }

    P2SlabPool *pools = P2SLAB_POOL(self->_pools);
    P2SlabPool *pool = &pools[order];

    if (pool->objects == NULL) {
        P2SlabPage *slab = create_p2slab(self, order);
        if (slab == NULL) {
            printf("p2slab alloc failed, cannot create new slab\n");
            return NULL;
        }
        slab->next = pool->pages;
        pool->pages = slab;
    }

    P2SlabObject *node = p2slab_dequeue(pool);
    const unsigned int space = MEMORY_SPACE_STD(P2SlabObject);
    self->usage += 1 << order;
    return (void *) ((size_t) node + space);
}

char p2slab_free(P2SlabMemory *self, void **ptr) {
#if MEM_DEBUG_MODE
    if (self == NULL) {
        printf("p2slab free failed, invalid instance\n");
        return 0;
    }
    if (ptr == NULL || (*ptr) == NULL) {
        printf("p2slab free failed, invalid pointer\n");
        return 0;
    }
#endif
    const unsigned int space = MEMORY_SPACE_STD(P2SlabObject);
    P2SlabObject *node = (P2SlabObject *) ((size_t) (*ptr) - space);

    if (!BYTE6AB_GET_A(node->next)) {
        printf("p2slab free failed, already freed\n");
        return 0;
    }
    unsigned int order = BYTE6AB_GET_B(node->next);
    P2SlabPool *pools = P2SLAB_POOL(self->_pools);
    p2slab_enqueue(&pools[order], node, order);
    self->usage -= (1 << order);
    return 1;
}

char p2slab_is_free(P2SlabPage *slab, unsigned int objectSize) {
    unsigned int space = MEMORY_SPACE_STD(P2SlabPage);
    const size_t start = (size_t) slab - slab->padding;
    size_t cursor = slab->padding + space;
    while (1) {
        size_t address = start + cursor;
        space = MEMORY_SPACE_STD(P2SlabObject);
        unsigned int padding = MEMORY_ALIGNMENT_STD(address, P2SlabObject);
        cursor += padding + objectSize;
        if (cursor > slab->size)
            break;
        P2SlabObject *obj = (P2SlabObject *) (address + padding - space);
        if (BYTE6AB_GET_A(obj->next)) return 0;
    }
    return 1;
}

void p2slab_fit_pool(P2SlabMemory *self, P2SlabPool *pool, unsigned int order) {
    if (pool->pages == NULL)
        return;
    unsigned int objectSize = 1 << order;
    P2SlabPage *slab = pool->pages;
    pool->pages = NULL;
    pool->objects = NULL;
    while (slab != NULL) {
        P2SlabPage *next = slab->next;

        if (p2slab_is_free(slab, objectSize)) {

            unsigned int size = self->_n * objectSize;
            size += MEMORY_SPACE_STD(P2SlabPage);
            size += self->_n * MEMORY_SPACE_STD(P2SlabObject);
            size += sizeof(size_t);

            self->total -= size;
            destroy_p2slab(self, slab);

        } else {
            unsigned int space = MEMORY_SPACE_STD(P2SlabPage);
            const size_t start = (size_t) slab - slab->padding;
            size_t cursor = slab->padding + space;
            while (1) {
                size_t address = start + cursor;
                space = MEMORY_SPACE_STD(P2SlabObject);
                unsigned int padding = MEMORY_ALIGNMENT_STD(address, P2SlabObject);
                cursor += padding + objectSize;
                if (cursor > slab->size)
                    break;

                P2SlabObject *obj = (P2SlabObject *) (address + padding - space);
                if (!BYTE71_GET_1(obj->next)) { // is free
                    p2slab_enqueue(pool, obj, order);
                } else {
                    self->usage += objectSize;
                }
            }
            slab->next = pool->pages;
            pool->pages = slab;
        }
        slab = next;
    }
}

void p2slab_fit(P2SlabMemory *self) {
    self->usage = 0;
    P2SlabPool *pools = P2SLAB_POOL(self->_pools);
    for (int i = 0; i < P2SLAB_MAX; i++) {
        p2slab_fit_pool(self, &pools[i], i);
    }
}