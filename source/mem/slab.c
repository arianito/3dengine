#include "mem/slab.h"

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

#include "mem/utils.h"

typedef struct {
    size_t next;
} SlabObject;

typedef struct {
    void *next;
    unsigned int size;
    unsigned int padding;
} SlabPage;

void slab_enqueue(SlabMemory *self, SlabObject *node) {
    node->next = BYTE71((size_t) self->_objects, 0);
    self->_objects = node;
}

SlabObject *slab_dequeue(SlabMemory *self) {
    if (self->_objects == NULL)
        return NULL;

    SlabObject *node = self->_objects;
    node->next = BYTE71_SET_1(node->next, 1);
    self->_objects = (SlabObject *) (BYTE71_GET_7(node->next));
    return node;
}

SlabPage *create_slab(SlabMemory *self) {
    unsigned int n = self->_slabSize / self->_objectSize;

    unsigned int size = self->_slabSize;
    size += MEMORY_SPACE_STD(SlabPage);
    size += n * MEMORY_SPACE_STD(SlabObject);
    size += sizeof(size_t);

    self->total += size;

    void *m = NULL;
    if (self->_allocator.alloc != NULL)
        m = self->_allocator.alloc(size);
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
        cursor += padding + self->_objectSize;
        if (cursor > size)
            break;

        slab_enqueue(self, (SlabObject *) (address + padding - space));
    }
    return slab;
}

void destroy_slab(SlabMemory *self, SlabPage *slab) {
    size_t op = (size_t) slab - slab->padding;
    if (self->_allocator.free != NULL)
        self->_allocator.free((void *) (op));
    else
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
    self->_pages = NULL;
    self->_objects = NULL;
    self->_allocator.alloc = NULL;
    self->_allocator.free = NULL;
    self->_padding = padding;
    self->usage = 0;
    self->total = padding + sizeof(SlabMemory);

    self->_slabSize = slabSize;
    self->_objectSize = objectSize;
    return self;
}

SlabMemory *slab_create_alloc(GeneralAllocator allocator, unsigned int slabSize, unsigned short objectSize) {
    void *m = allocator.alloc(sizeof(SlabMemory));
    if (m == NULL) {
        printf("slab: make failed, system can't provide free memory\n");
        exit(EXIT_FAILURE);
    }
    SlabMemory *slab = slab_create(m, slabSize, objectSize);
    slab->_allocator = allocator;
    return slab;
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

    SlabPage *slab = (*self)->_pages;
    while (slab != NULL) {
        void *next = slab->next;
        destroy_slab(*self, slab);
        slab = next;
    }

    size_t op = (size_t) (*self) - (*self)->_padding;

    if ((*self)->_allocator.free != NULL)
        (*self)->_allocator.free((void *) (op));
    else
        free((void *) (op));
    (*self) = NULL;
}

void *slab_alloc(SlabMemory *self) {
#if MEM_DEBUG_MODE
    if (self == NULL) {
        printf("slab: alloc failed, invalid instance\n");
        return NULL;
    }
#endif
    if (self->_objects == NULL) {
        SlabPage *slab = create_slab(self);
        if (slab == NULL) {
            printf("slab: alloc failed, cannot create new slab\n");
            return NULL;
        }
        slab->next = self->_pages;
        self->_pages = slab;
    }

    SlabObject *node = slab_dequeue(self);
    const unsigned int space = MEMORY_SPACE_STD(SlabObject);
    self->usage += self->_objectSize;
    return (void *) ((size_t) node + space);
}

char slab_free(SlabMemory *self, void **ptr) {
#if MEM_DEBUG_MODE
    if (self == NULL) {
        printf("slab: free failed, invalid instance\n");
        return 0;
    }
    if (ptr == NULL || (*ptr) == NULL) {
        printf("slab: free failed, invalid pointer\n");
        return 0;
    }
#endif
    const unsigned int space = MEMORY_SPACE_STD(SlabObject);
    SlabObject *node = (SlabObject *) ((size_t) (*ptr) - space);

    if (!BYTE71_GET_1(node->next)) {
        printf("slab: free failed, already freed\n");
        return 0;
    }

    slab_enqueue(self, node);
    self->usage -= self->_objectSize;
    return 1;
}

char slab_is_free(SlabPage *slab, unsigned int objectSize) {
    unsigned int space = MEMORY_SPACE_STD(SlabPage);
    const size_t start = (size_t) slab - slab->padding;
    size_t cursor = slab->padding + space;
    while (1) {
        size_t address = start + cursor;
        space = MEMORY_SPACE_STD(SlabObject);
        unsigned int padding = MEMORY_ALIGNMENT_STD(address, SlabObject);
        cursor += padding + objectSize;
        if (cursor > slab->size)
            break;
        SlabObject *obj = (SlabObject *) (address + padding - space);
        if (BYTE71_GET_1(obj->next)) return 0;
    }
    return 1;
}

void slab_fit(SlabMemory *self) {

    SlabPage *slab = self->_pages;
    self->_pages = NULL;
    self->_objects = NULL;
    self->usage = 0;

    while (slab != NULL) {
        SlabPage *next = slab->next;

        if (slab_is_free( slab, self->_objectSize)) {
            unsigned int n = self->_slabSize / self->_objectSize;
            unsigned int size = self->_slabSize;
            size += MEMORY_SPACE_STD(SlabPage);
            size += n * MEMORY_SPACE_STD(SlabObject);
            size += sizeof(size_t);
            self->total -= size;
            destroy_slab(self, slab);

        } else {
            unsigned int space = MEMORY_SPACE_STD(SlabPage);
            const size_t start = (size_t) slab - slab->padding;
            size_t cursor = slab->padding + space;
            while (1) {
                size_t address = start + cursor;
                space = MEMORY_SPACE_STD(SlabObject);
                unsigned int padding = MEMORY_ALIGNMENT_STD(address, SlabObject);
                cursor += padding + self->_objectSize;
                if (cursor > slab->size)
                    break;

                SlabObject *obj = (SlabObject *) (address + padding - space);
                if (!BYTE71_GET_1(obj->next)) { // is free
                    slab_enqueue(self, obj);
                } else {
                    self->usage += self->_objectSize;
                }
            }
            slab->next = self->_pages;
            self->_pages = slab;
        }
        slab = next;
    }
}