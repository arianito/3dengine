#include "mem/slab.h"

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "mem/utils.h"

void slab_enqueue(SlabMemory *self, SlabObject *node) {
    node->next = BYTE71((size_t) self->objects, 0);
    self->objects = node;
}

SlabObject *slab_dequeue(SlabMemory *self) {
    if (self->objects == NULL)
        return NULL;

    SlabObject *node = self->objects;
    node->next = BYTE71_SET_1(node->next, 1);
    self->objects = (SlabObject *) (BYTE71_GET_7(node->next));
    return node;
}

SlabPage *create_slab(SlabMemory *self) {
    int n = self->slabSize / self->objectSize;

    unsigned int size = self->slabSize;
    size += MEMORY_SPACE_STD(SlabPage);
    size += n * sizeof(SlabObject);
    size += sizeof(size_t);

    self->bytes += size;

    void *m = NULL;
    if (self->allocator != NULL)
        m = self->allocator(size);
    else
        m = malloc(size);
    if (m == NULL) {
        printf("slab: create slab failed, system can't provide free memory\n");
        return NULL;
    }
    const size_t start = (size_t) m;
    unsigned int space = MEMORY_SPACE_STD(SlabPage);
    unsigned int padding = MEMORY_PADDING_STD(start);

    SlabPage *slab = (SlabPage *) (start + padding);
    slab->next = NULL;
    slab->size = size;
    slab->padding = padding;

    size_t cursor = padding + space;
    space = MEMORY_SPACE_STD(SlabObject);
    while (1) {
        size_t address = start + cursor;
        padding = MEMORY_ALIGNMENT_STD(address, SlabObject);
        cursor += padding + self->objectSize;
        if (cursor > size)
            break;

        slab_enqueue(self, (SlabObject *) (address + padding - space));
        self->capacity++;
    }
    return slab;
}

void free_slab(SlabPage *slab) {
    size_t op = (size_t) slab - slab->padding;
    free((void *) (op));
}

SlabMemory *slab_create(void *m, unsigned int slabSize, unsigned short objectSize) {
    if (slabSize % objectSize != 0) {
        printf("slab: create failed, invalid chunk size\n");
        exit(EXIT_FAILURE);
    }
    const size_t start = (size_t) m;
    const unsigned int padding = MEMORY_PADDING_STD(start);

    SlabMemory *self = (SlabMemory *) (start + padding);
    self->pages = NULL;
    self->objects = NULL;
    self->allocator = NULL;
    self->padding = padding;
    self->usage = 0;
    self->capacity = 0;
    self->bytes = padding + sizeof(SlabMemory);

    self->slabSize = slabSize;
    self->objectSize = objectSize;
    return self;
}

SlabMemory *make_slab(unsigned int slabSize, unsigned short objectSize) {
    void *m = malloc(sizeof(SlabMemory));
    if (m == NULL) {
        printf("slab: make failed, system can't provide free memory\n");
        exit(EXIT_FAILURE);
    }
    return slab_create(m, slabSize, objectSize);
}

void slab_destroy(SlabMemory **self) {
    if (self == NULL || *self == NULL) {
        printf("slab: destroy failed, invalid instance\n");
        return;
    }

    SlabPage *slab = (*self)->pages;
    while (slab != NULL) {
        void *next = slab->next;

        size_t op = (size_t) slab - slab->padding;
        free((void *) (op));

        slab = next;
    }

    size_t op = (size_t) (*self) - (*self)->padding;
    free((void *) (op));
    (*self) = NULL;
}

void *slab_alloc(SlabMemory *self) {
    if (self == NULL) {
        printf("slab: alloc failed, invalid instance\n");
        return NULL;
    }
    if (self->objects == NULL) {
        SlabPage *slab = create_slab(self);
        if (slab == NULL) {
            printf("slab: alloc failed, cannot create new slab\n");
            return NULL;
        }
        slab->next = self->pages;
        self->pages = slab;
    }

    SlabObject *node = slab_dequeue(self);
    const unsigned int space = MEMORY_SPACE_STD(SlabObject);
    self->usage++;
    return (void *) ((size_t) node + space);
}

char slab_free(SlabMemory *self, void **ptr) {

    if (self == NULL) {
        printf("slab: free failed, invalid instance\n");
        return 0;
    }
    if (ptr == NULL || (*ptr) == NULL) {
        printf("slab: free failed, invalid pointer\n");
        return 0;
    }
    const unsigned int space = sizeof(SlabObject);
    SlabObject *node = (SlabObject *) ((size_t) (*ptr) - space);

    if (!BYTE71_GET_1(node->next)) {
        printf("slab: free failed, already freed\n");
        return 0;
    }

    slab_enqueue(self, node);
    self->usage--;
    return 1;
}