
#include "mem/stack.h"

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

#include "mem/utils.h"

typedef struct {
    size_t next; // 7bytes offset 1byte padding
} StackMemoryNode;

void *stack_alloc(StackMemory *self, unsigned int size, unsigned int alignment) {
#if MEM_DEBUG_MODE
    if (!ISPOW2(alignment)) {
        printf("stack: alloc failed, invalid alignment\n");
        return NULL;
    }
    if (self == NULL) {
        printf("stack: alloc failed, invalid instance\n");
        return NULL;
    }
#endif
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
    node->next = BYTE71((size_t) self->_head, padding);
    self->_head = node;

    return (void *) (address + padding);
}

char stack_free(StackMemory *self, void **p) {
#if MEM_DEBUG_MODE
    if (self == NULL) {
        printf("stack: free failed, invalid instance\n");
        return 0;
    }
    if (p == NULL || (*p) == NULL) {
        printf("stack: free failed, invalid pointer\n");
        return 0;
    }
#endif
    size_t start = (size_t) self - self->_padding;
    size_t address = (size_t) (*p);

    const unsigned int space = MEMORY_SPACE_STD(StackMemoryNode);
    StackMemoryNode *node = (StackMemoryNode *) (address - space);
    if (node != self->_head) {
        printf("stack: free failed, you must free the stack in order\n");
        return 0;
    }

    void *next = (void *) BYTE71_GET_7(node->next);
    unsigned char padding = BYTE71_GET_1(node->next);

    self->usage = (address - padding) - start;
    self->_head = next;
    *p = NULL;
    return 1;
}

char stack_pop(StackMemory *self) {
#if MEM_DEBUG_MODE
    if (self == NULL) {
        printf("stack: free failed, invalid instance\n");
        return 0;
    }
    if (self->_head == NULL) {
        printf("stack: free failed, invalid pointer\n");
        return 0;
    }
#endif
    const unsigned int space = MEMORY_SPACE_STD(StackMemoryNode);
    StackMemoryNode *node = (StackMemoryNode *) self->_head;


    size_t start = (size_t) self - self->_padding;
    size_t address = (size_t) node + space;

    void *next = (void *) BYTE71_GET_7(node->next);
    unsigned char padding = BYTE71_GET_1(node->next);

    self->usage = (address - padding) - start;
    self->_head = next;
    return 1;
}

void stack_reset(StackMemory *self) {
#if MEM_DEBUG_MODE
    if (self == NULL) {
        printf("stack: reset failed, invalid instance\n");
        return;
    }
#endif
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

char stack_expand(StackMemory *self, unsigned int newSize) {
    if (self == NULL) {
        printf("stack: expand failed, invalid instance\n");
        return 0;
    }
    if (self->_head == NULL) {
        printf("stack: expand failed, no prior alloc\n");
        return 0;
    }
    const unsigned int space = MEMORY_SPACE_STD(StackMemoryNode);
    size_t start = (size_t) self - self->_padding;
    unsigned int prevSize = self->usage - ((size_t) self->_head + space - start);
    self->usage += (newSize - prevSize);
    return 1;
}

unsigned int stack_n(StackMemory *self) {
    if (self->_head == NULL)
        return 0;
    const unsigned int space = MEMORY_SPACE_STD(StackMemoryNode);
    size_t start = (size_t) self - self->_padding;
    return self->usage - ((size_t) self->_head + space - start);
}
