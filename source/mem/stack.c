
#include "mem/stack.h"

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

#include "mem/utils.h"

typedef struct {
    size_t data; // 7bytes offset 1byte padding
} StackMemoryNode;

void *stack_alloc(StackMemory *self, unsigned int size, unsigned int alignment) {
    if (!ISPOW2(alignment)) {
        printf("stack: alloc failed, invalid alignment\n");
        return NULL;
    }
    if (self == NULL) {
        printf("stack: alloc failed, invalid instance\n");
        return NULL;
    }
    size_t start = (size_t) self - self->_padding;
    size_t address = start + self->usage;
    const unsigned int padding = MEMORY_ALIGNMENT(address, sizeof(StackMemoryNode), alignment);
    const unsigned int space = MEMORY_SPACE(sizeof(StackMemoryNode), sizeof(size_t));
    if (self->usage + padding + size > self->total) {
        printf("stack: alloc failed, insufficient memory\n");
        return NULL;
    }
    self->usage += padding + size;

    StackMemoryNode *node = (StackMemoryNode *) (address + padding - space);

    if (self->_head == NULL)
        node->data = BYTE71(0, padding);
    else
        node->data = BYTE71((size_t) self->_head - start, padding);

    self->_head = node;

    return (void *) (address + padding);
}

char stack_free(StackMemory *self, void **p) {
    if (self == NULL) {
        printf("stack: free failed, invalid instance\n");
        return 0;
    }
    if (p == NULL || (*p) == NULL) {
        printf("stack: free failed, invalid pointer\n");
        return 0;
    }
    size_t start = (size_t) self - self->_padding;
    size_t address = (size_t) (*p);
    size_t end = start + self->total;
    if (!(address >= start && address < end)) {
        printf("stack: free failed, out of boundary\n");
        return 0;
    }
    if (address >= start + self->usage) {
        printf("stack: free failed, invalid order of free\n");
        return 0;
    }
    const unsigned int space = MEMORY_SPACE_STD(StackMemoryNode);
    StackMemoryNode *node = (StackMemoryNode *) (address - space);
    if (node != self->_head) {
        printf("stack: free failed, you must free the stack in order\n");
        return 0;
    }

    size_t offset = BYTE71_GET_7(node->data);
    unsigned char padding = BYTE71_GET_1(node->data);

    self->usage = (address - padding) - start;
    StackMemoryNode *head = (StackMemoryNode *) self->_head;

    offset = BYTE71_GET_7(head->data);

    if (offset == 0)
        self->_head = NULL;
    else
        self->_head = (StackMemoryNode *) (start + offset);
    *p = NULL;
    return 1;
}

void stack_reset(StackMemory *self) {
    if (self == NULL) {
        printf("stack: reset failed, invalid instance\n");
        return;
    }
    const unsigned int space = MEMORY_SPACE_STD(StackMemory);
    self->usage = self->_padding + space;
    self->_head = NULL;
}

void stack_destroy(StackMemory **self) {
    if (self == NULL || *self == NULL) {
        printf("stack: destroy failed, invalid instance\n");
        return;
    }
    size_t op = (size_t) (*self) - (*self)->_padding;
    free((void *) (op));
    *self = NULL;
}

StackMemory *stack_create(void *m, unsigned int size) {
    size_t address = (size_t) m;
    const unsigned int space = MEMORY_SPACE_STD(StackMemory);
    const unsigned int padding = MEMORY_PADDING_STD(address);
    StackMemory *self = (StackMemory *) (address + padding);
    self->_head = NULL;
    self->total = size;
    self->usage = padding + space;
    self->_padding = padding;
    return self;
}

StackMemory *make_stack(unsigned int size) {
    void *m = malloc(size);
    if (m == NULL) {
        printf("stack: make failed, system can't provide free memory\n");
        exit(EXIT_FAILURE);
    }
    return stack_create(m, size);
}
