
#include "mem/freelist.h"

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

#include "mem/utils.h"

#define NODE_LOWER(node) ((size_t)(node) + MEMORY_SPACE_STD(FreeListMemory) - (node)->_padding)
#define NODE_HIGHER(node) ((size_t)(node) + MEMORY_SPACE_STD(FreeListMemory))

void freelist_first(FreeListMemory *self, unsigned int size, unsigned int alignment, unsigned int *outPadding,
                    FreeListMemory **outPrevNode, FreeListMemory **outNode) {
    FreeListMemory
            *node = self->_next,
            *prev = NULL;
    unsigned int padding;
    while (node != NULL) {
        padding = MEMORY_ALIGNMENT(NODE_LOWER(node), sizeof(FreeListMemory), alignment);
        unsigned int space = size + padding + MEMORY_SPACE_STD(FreeListMemory);
        if (node->total > space)
            break;

        prev = node;
        node = node->_next;
    }
    *outPrevNode = prev;
    *outNode = node;
    *outPadding = padding;
}

void freelist_best(FreeListMemory *self, unsigned int size, unsigned int alignment, unsigned int *outPadding,
                   FreeListMemory **outPrevNode, FreeListMemory **outNode) {
    FreeListMemory
            *node = self->_next,
            *best = NULL,
            *prev = NULL,
            *bestPrev = NULL;
    unsigned int padding, bestPad = 0;
    int min = (~(0) - 1);
    while (node != NULL) {
        padding = MEMORY_ALIGNMENT(NODE_LOWER(node), sizeof(FreeListMemory), alignment);
        size_t space = size + padding + MEMORY_SPACE_STD(FreeListMemory);
        if (node->total > space && (node->total - space) < min) {
            best = node;
            bestPrev = prev;
            bestPad = padding;
            min = node->total - space;
        }
        prev = node;
        node = node->_next;
    }
    *outNode = best;
    *outPrevNode = bestPrev;
    *outPadding = bestPad;
}

void freelist_insert(FreeListMemory *self, FreeListMemory *prevNode, FreeListMemory *newNode) {
    if (prevNode == NULL) {
        newNode->_next = self->_next;
        self->_next = newNode;
        return;
    }
    newNode->_next = prevNode->_next;
    prevNode->_next = newNode;
}

void freelist_remove(FreeListMemory *self, FreeListMemory *prevNode, FreeListMemory *node) {
    if (prevNode == NULL) {
        self->_next = node->_next;
        return;
    }
    prevNode->_next = node->_next;
}

void freelist_joinnext(FreeListMemory *self, FreeListMemory *previousNode, FreeListMemory *freeNode) {
    FreeListMemory *next = (FreeListMemory *) freeNode->_next;
    if (next != NULL && NODE_LOWER(freeNode) + freeNode->total == NODE_LOWER(next)) {
        freeNode->total += next->total;
        freelist_remove(self, freeNode, freeNode->_next);
    }

    if (previousNode != NULL &&
        NODE_LOWER(previousNode) + previousNode->total == NODE_LOWER(freeNode)) {
        previousNode->total += freeNode->total;
        freelist_remove(self, previousNode, freeNode);
    }
}

void *freelist_alloc(FreeListMemory *self, unsigned int size, unsigned int alignment) {
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

    unsigned int requiredSpace = size + padding;
    unsigned int remainingSpace = node->total - requiredSpace;

    size_t addr = NODE_LOWER(node);
    void *tmp = node->_next;
    node = (FreeListMemory *) (addr + padding - space);
    node->_padding = padding;
    node->_next = tmp;

    node->total = requiredSpace;

    if (remainingSpace > 0) {
        FreeListMemory *newNode = (FreeListMemory *) (addr + requiredSpace);
        newNode->_padding = space;
        newNode->total = remainingSpace;
        freelist_insert(self, node, newNode);
    }
    freelist_remove(self, prevNode, node);

    return (void *) NODE_HIGHER(node);
}

char freelist_free(FreeListMemory *self, void **ptr) {

    if (self == NULL) {
        printf("freelist: free failed, invalid instance\n");
        return 0;
    }
    if (ptr == NULL || (*ptr) == NULL) {
        printf("freelist: free failed, invalid pointer\n");
        return 0;
    }

    unsigned int space = MEMORY_SPACE_STD(FreeListMemory);

    FreeListMemory *freeedNode = (FreeListMemory *) ((size_t) (*ptr) - space);
    freeedNode->_next = NULL;

    FreeListMemory *prevNode = NULL;
    FreeListMemory *node = self->_next;

    while (node != NULL) {
        if (*ptr < (void *) node) {
            freelist_insert(self, prevNode, freeedNode);
            break;
        }
        prevNode = node;
        node = node->_next;
    }

    freelist_joinnext(self, prevNode, freeedNode);

    if (prevNode == NULL && freeedNode == self->_next) {
        size_t lower = NODE_LOWER(freeedNode);
        void *tmp = freeedNode->_next;
        unsigned int size = freeedNode->total;

        node = (FreeListMemory *) (lower);
        node->_padding = space;
        node->total = size;
        node->_next = tmp;
        self->_next = node;
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
    self->_next = NULL;
    node->_padding = padding;
    node->total = self->total - (self->_padding);
    node->_next = NULL;
    freelist_insert(self, NULL, node);
}

unsigned int freelist_usage(FreeListMemory *self) {
    unsigned int sum = 0;
    FreeListMemory *node = self->_next;
    while (node != NULL) {
        sum += node->total;
        node = node->_next;
    }
    return self->total - sum;
}

FreeListMemory *freelist_create(void *m, unsigned int size) {
    size_t address = (size_t) m;
    const unsigned int space = MEMORY_SPACE_STD(FreeListMemory);
    const unsigned int padding = MEMORY_ALIGNMENT_STD(address, FreeListMemory);
    FreeListMemory *self = (FreeListMemory *) (address + padding - space);
    self->total = size;
    self->_padding = padding;

    freelist_reset(self);
    return self;
}

FreeListMemory *make_freelist(unsigned int size) {
    void *m = malloc(size);
    if (m == NULL) {
        printf("freelist: make failed, system can't provide free memory\n");
        exit(EXIT_FAILURE);
    }
    return freelist_create(m, size);
}