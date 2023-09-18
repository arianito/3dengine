#pragma once


#include <stddef.h>

typedef struct {
    void *head;
    size_t size;
    size_t offset;
    size_t peak;
    size_t padding;
} StackMemory;

typedef struct {
    size_t data; // 7bytes offset 1byte padding
} StackMemoryNode;

void *stack_alloc(StackMemory *self, size_t size, unsigned int alignment);

unsigned char stack_free(StackMemory *self, void **ptr);

void stack_reset(StackMemory *self);

void stack_destroy(StackMemory **self);

StackMemory *stack_create(void *m, size_t size);

StackMemory *make_stack(size_t size);

StackMemory *make_stack_exact(size_t size);