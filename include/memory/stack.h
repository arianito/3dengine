#pragma once

#include <stddef.h>

typedef struct
{
	void *head;
	size_t size;
	size_t offset;
	size_t peak;
	size_t padding;
} StackMemory;

typedef struct
{
	void *next;
	size_t padding;
} StackMemoryNode;

StackMemory *make_stack_from(void *ptr, size_t size);
StackMemory *make_stack(size_t size);
void *stack_alloc(StackMemory *self, size_t size, int alignment);
void stack_free(StackMemory *self, void **ptr);
void stack_reset(StackMemory *self);
void stack_destroy(StackMemory **self);
