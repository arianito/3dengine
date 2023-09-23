#include "mem/p2slab.h"

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

#include "mem/utils.h"

// a used
// b order
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

    unsigned int size = self->n * objectSize;
    size += MEMORY_SPACE_STD(P2SlabPage);
    size += self->n * MEMORY_SPACE_STD(P2SlabObject);
    size += sizeof(size_t);

    self->total += size;

    void *m = NULL;
    if (self->allocator.alloc != NULL)
        m = self->allocator.alloc(size);
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

        p2slab_enqueue(&self->pools[order], (P2SlabObject *) (address + padding - space), order);
    }
    return slab;
}

P2SlabMemory *p2slab_create(void *m, unsigned int n) {
    const size_t start = (size_t) m;
    const unsigned int padding = MEMORY_PADDING_STD(start);

    P2SlabMemory *self = (P2SlabMemory *) (start + padding);
    self->allocator.alloc = NULL;
    self->allocator.free = NULL;
    self->n = n;
    self->padding = padding;
    self->usage = 0;
    self->total = padding + sizeof(P2SlabMemory);
    for (int i = 0; i < 32; i++) {
        self->pools[i].objects = NULL;
        self->pools[i].pages = NULL;
    }
    return self;
}

P2SlabMemory *p2slab_create_alloc(P2SlabAllocator allocator, unsigned int n) {
    void *m = allocator.alloc(sizeof(P2SlabMemory));
    if (m == NULL) {
        printf("p2slab make failed, system can't provide free memory\n");
        exit(EXIT_FAILURE);
    }
    P2SlabMemory *slab = p2slab_create(m, n);
    slab->allocator = allocator;
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

void p2slab_destroy(P2SlabMemory **self) {
    if (self == NULL || *self == NULL) {
        printf("p2slab destroy failed, invalid instance\n");
        return;
    }

    for (int i = 0; i < 32; i++) {
        P2SlabPage *slab = (*self)->pools[i].pages;
        while (slab != NULL) {
            void *next = slab->next;

            size_t op = (size_t) slab - slab->padding;
            if ((*self)->allocator.free != NULL)
                (*self)->allocator.free((void *) (op));
            else
                free((void *) (op));

            slab = next;
        }
    }

    size_t op = (size_t) (*self) - (*self)->padding;

    if ((*self)->allocator.free != NULL)
        (*self)->allocator.free((void *) (op));
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
    if (self == NULL) {
        printf("p2slab alloc failed, invalid instance\n");
        return NULL;
    }
    unsigned int order = 0;
    size = p2slab_next((int) size);
    while (size > 1) {
        size >>= 1;
        order++;
    }

    P2SlabPool *pool = &self->pools[order];

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

    if (self == NULL) {
        printf("p2slab free failed, invalid instance\n");
        return 0;
    }
    if (ptr == NULL || (*ptr) == NULL) {
        printf("p2slab free failed, invalid pointer\n");
        return 0;
    }
    const unsigned int space = MEMORY_SPACE_STD(P2SlabObject);
    P2SlabObject *node = (P2SlabObject *) ((size_t) (*ptr) - space);

    if (!BYTE6AB_GET_A(node->next)) {
        printf("p2slab free failed, already freed\n");
        return 0;
    }
    unsigned int order = BYTE6AB_GET_B(node->next);
    P2SlabPool *pool = &self->pools[order];
    p2slab_enqueue(pool, node, order);
    self->usage -= (1 << order);
    return 1;
}