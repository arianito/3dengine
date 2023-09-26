#pragma once


#include <stddef.h>

typedef struct __attribute__((aligned(32), packed)) {
    void *_head;
    unsigned int _padding;
    unsigned int total;
    unsigned int usage;
} StackMemory;

StackMemory *stack_create(void *m, unsigned int size);

StackMemory *make_stack(unsigned int size);

void stack_reset(StackMemory *self);

void stack_destroy(StackMemory **self);


void *stack_alloc(StackMemory *self, unsigned int size, unsigned int alignment);

char stack_expand(StackMemory *self, unsigned int newSize);

char stack_free(StackMemory *self, void **ptr);
char stack_pop(StackMemory *self);

unsigned int stack_n(StackMemory *self);