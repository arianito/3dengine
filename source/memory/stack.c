
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "memory/stack.h"
#include "memory/utils.h"

void *stack_alloc(StackMemory *self, size_t size, int alignment)
{
	assert((alignment & (alignment - 1)) == 0 && "invalid alignment");
	if (self == NULL)
	{
		printf("stack: invalid instance\n");
		exit(EXIT_FAILURE);
		return NULL;
	}
	StackMemory *instance = self;
	size_t address = ((size_t)instance - instance->padding) + instance->offset;
	int padding = calculate_alignment(address, sizeof(StackMemoryNode), alignment);
	int space = calculate_space(sizeof(StackMemoryNode), sizeof(size_t));
	if (instance->offset + padding + size > instance->size)
	{
		printf("stack: insufficient memory\n");
		exit(EXIT_FAILURE);
		return NULL;
	}
	address += padding;
	instance->offset += padding + size;
	if (instance->offset > instance->peak)
		instance->peak = instance->offset;
	StackMemoryNode *node = (StackMemoryNode *)(address - space);
	node->next = instance->head;
	node->padding = padding;
	instance->head = node;
	return (void *)address;
}

void stack_free(StackMemory *self, void **p)
{
	if (self == NULL || p == NULL || (*p) == NULL)
		return;
	StackMemory *instance = self;
	size_t start = (size_t)instance - instance->padding;
	size_t address = (size_t)(*p);
	size_t end = start + instance->size;
	if (!(address >= start || address < end))
		return;
	if (address >= start + instance->offset)
		return;
	int space = calculate_space(sizeof(StackMemoryNode), sizeof(size_t));
	StackMemoryNode *node = (StackMemoryNode *)(address - space);
	if (node != instance->head)
		return;
	instance->offset = (address - start) - node->padding;
	instance->head = ((StackMemoryNode *)instance->head)->next;
	*p = NULL;
}

void stack_reset(StackMemory *self)
{
	if (self == NULL)
		return;
	StackMemory *instance = self;
	int space = calculate_space(sizeof(StackMemory), sizeof(size_t));
	instance->offset = instance->padding + space;
	instance->peak = instance->offset;
	instance->head = NULL;
}

void stack_destroy(StackMemory **self)
{
	if (self == NULL || *self == NULL)
		return;
	StackMemory *instance = *self;
	size_t op = (size_t)instance - instance->padding;
	free((void *)(op));
	*self = NULL;
}

StackMemory *make_stack_from(void *m, size_t size)
{
	size_t address = (size_t)m;
	int space = calculate_space(sizeof(StackMemory), sizeof(size_t));
	int padding = calculate_padding(address, sizeof(size_t));
	StackMemory *instance = (StackMemory *)(address + padding);
	instance->head = NULL;
	instance->size = size;
	instance->offset = padding + space;
	instance->peak = instance->offset;
	instance->padding = padding;
	return instance;
}
StackMemory *make_stack(size_t size)
{
	void *m = malloc(size);
	if (m == NULL)
	{
		printf("stack: system can't provide free memory\n");
		exit(EXIT_FAILURE);
		return NULL;
	}
	return make_stack_from(m, size);
}
