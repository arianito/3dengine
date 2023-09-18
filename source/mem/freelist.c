
#include "mem/freelist.h"

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "mem/utils.h"

#define NODE_LOWER(node) ((size_t)(node) + MEMORY_SPACE_STD(FreeListMemory) - (node)->padding)
#define NODE_HIGHER(node) ((size_t)(node) + MEMORY_SPACE_STD(FreeListMemory))

void
freelist_first(FreeListMemory *self, size_t size, unsigned int alignment, unsigned int *outPadding,
               FreeListMemory **outPrevNode, FreeListMemory **outNode) {
    FreeListMemory
            *node = self->next,
            *prev = NULL;
    unsigned int padding;
    while (node != NULL) {
        padding = MEMORY_ALIGNMENT(NODE_LOWER(node), sizeof(FreeListMemory), alignment);
        size_t space = size + padding + MEMORY_SPACE_STD(FreeListMemory);
        if (node->size > space)
            break;

        prev = node;
        node = node->next;
    }
    *outPrevNode = prev;
    *outNode = node;
    *outPadding = padding;
}

void
freelist_best(FreeListMemory *self, size_t size, unsigned int alignment, unsigned int *outPadding,
              FreeListMemory **outPrevNode, FreeListMemory **outNode) {
    FreeListMemory
            *node = self->next,
            *best = NULL,
            *prev = NULL,
            *bestPrev = NULL;
    unsigned int padding, bestPad = 0;
    size_t min = (~(0ULL) - 1);
    while (node != NULL) {
        padding = MEMORY_ALIGNMENT(NODE_LOWER(node), sizeof(FreeListMemory), alignment);
        size_t space = size + padding + MEMORY_SPACE_STD(FreeListMemory);
        if (node->size > space && (node->size - space) < min) {
            best = node;
            bestPrev = prev;
            bestPad = padding;
            min = node->size - space;
        }
        prev = node;
        node = node->next;
    }
    *outNode = best;
    *outPrevNode = bestPrev;
    *outPadding = bestPad;
}

void freelist_insert(FreeListMemory *self, FreeListMemory *prevNode, FreeListMemory *newNode) {
    if (prevNode == NULL) {
        newNode->next = self->next;
        self->next = newNode;
        return;
    }
    newNode->next = prevNode->next;
    prevNode->next = newNode;
}

void freelist_remove(FreeListMemory *self, FreeListMemory *prevNode, FreeListMemory *node) {
    if (prevNode == NULL) {
        self->next = node->next;
        return;
    }
    prevNode->next = node->next;
}

void
freelist_joinnext(FreeListMemory *self, FreeListMemory *previousNode, FreeListMemory *freeNode) {
    FreeListMemory *next = (FreeListMemory *) freeNode->next;
    if (next != NULL && NODE_LOWER(freeNode) + freeNode->size == NODE_LOWER(next)) {
        freeNode->size += next->size;
        freelist_remove(self, freeNode, freeNode->next);
    }

    if (previousNode != NULL &&
        NODE_LOWER(previousNode) + previousNode->size == NODE_LOWER(freeNode)) {
        previousNode->size += freeNode->size;
        freelist_remove(self, previousNode, freeNode);
    }
}

void *freelist_alloc(FreeListMemory *self, size_t size, unsigned int alignment) {
    if (!ISPOW2(alignment)) {
        printf("freelist: alloc failed, invalid alignment\n");
        return NULL;
    }
    if (self == NULL) {
        printf("freelist: alloc failed, invalid instance\n");
        return NULL;
    }
    unsigned int padding;
    FreeListMemory *prevNode;
    FreeListMemory *node;
    freelist_best(self, size, alignment, &padding, &prevNode, &node);

    if (node == NULL) {
        printf("freelist: alloc failed, insufficient memory\n");
        return NULL;
    }

    const unsigned int space = MEMORY_SPACE_STD(FreeListMemory);

    size_t requiredSpace = size + padding;
    size_t remainingSpace = node->size - requiredSpace;

    size_t addr = NODE_LOWER(node);
    void *tmp = node->next;
    node = (FreeListMemory *) (addr + padding - space);
    node->padding = padding;
    node->next = tmp;

    node->size = requiredSpace;

    if (remainingSpace > 0) {
        FreeListMemory *newNode = (FreeListMemory *) (addr + requiredSpace);
        newNode->padding = space;
        newNode->size = remainingSpace;
        freelist_insert(self, node, newNode);
    }
    freelist_remove(self, prevNode, node);

    return (void *) NODE_HIGHER(node);
}

unsigned char freelist_free(FreeListMemory *self, void **ptr) {

    if (self == NULL) {
        printf("freelist: free failed, invalid instance\n");
        return 0;
    }
    if (ptr == NULL || (*ptr) == NULL) {
        printf("freelist: free failed, invalid pointer\n");
        return 0;
    }

    size_t start = NODE_LOWER(self);
    size_t address = (size_t) (*ptr);
    size_t end = start + self->size;
    if (!(address >= start && address < end)) {
        printf("freelist: free failed, out of boundary\n");
        return 0;
    }

    unsigned int space = MEMORY_SPACE_STD(FreeListMemory);

    FreeListMemory *freeedNode = (FreeListMemory *) ((size_t) (*ptr) - space);
    freeedNode->next = NULL;

    FreeListMemory *prevNode = NULL;
    FreeListMemory *node = self->next;

    while (node != NULL) {
        if (*ptr < (void *) node) {
            freelist_insert(self, prevNode, freeedNode);
            break;
        }
        prevNode = node;
        node = node->next;
    }

    freelist_joinnext(self, prevNode, freeedNode);

    if (prevNode == NULL && freeedNode == self->next) {
        size_t lower = NODE_LOWER(freeedNode);
        void *tmp = freeedNode->next;
        size_t size = freeedNode->size;

        node = (FreeListMemory *) (lower);
        node->padding = space;
        node->size = size;
        node->next = tmp;
        self->next = node;
    }

    *ptr = NULL;
    return 1;
}

void freelist_destroy(FreeListMemory **self) {
    if (self == NULL || *self == NULL) {
        printf("freelist: destroy failed, invalid instance\n");
        return;
    }
    free((void *) NODE_LOWER(*self));
    *self = NULL;
}

void freelist_reset(FreeListMemory *self) {
    size_t start = NODE_HIGHER(self);
    const unsigned int padding = MEMORY_ALIGNMENT_STD(start, FreeListMemory);
    FreeListMemory *node = (FreeListMemory *) start;
    self->next = NULL;
    node->padding = padding;
    node->size = self->size - (self->padding);
    node->next = NULL;
    freelist_insert(self, NULL, node);
}

size_t freelist_capacity(FreeListMemory *self) {
    size_t sum = 0;
    FreeListMemory *node = self->next;
    while (node != NULL) {
        sum += node->size;
        node = node->next;
    }
    return self->size - sum;
}

FreeListMemory *freelist_create(void *m, size_t size) {
    size_t address = (size_t) m;
    const unsigned int space = MEMORY_SPACE_STD(FreeListMemory);
    const unsigned int padding = MEMORY_ALIGNMENT_STD(address, FreeListMemory);
    FreeListMemory *self = (FreeListMemory *) (address + padding - space);
    self->size = size;
    self->padding = padding;

    freelist_reset(self);
    return self;
}

FreeListMemory *make_freelist(size_t size) {
    void *m = malloc(size);
    if (m == NULL) {
        printf("freelist: make failed, system can't provide free memory\n");
        exit(EXIT_FAILURE);
    }
    return freelist_create(m, size);
}

FreeListMemory *make_freelist_exact(size_t size) {
    size += MEMORY_SPACE_STD(FreeListMemory) + sizeof(size_t);
    return make_freelist(size);
}