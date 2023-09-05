
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "memory/stack.h"
#include "memory/utils.h"


void *stack_alloc(StackMemory *self, size_t size, unsigned int alignment)
{
	if ((alignment & (alignment - 1)) != 0)
	{
		printf("stack: alloc failed, invalid alignment\n");
		return NULL;
	}
	if (self == NULL)
	{
		printf("stack: alloc failed, invalid instance\n");
		return NULL;
	}
	size_t start = (size_t)self - self->padding;
	size_t address = start + self->offset;
	unsigned int padding = calculate_alignment(address, sizeof(StackMemoryNode), alignment);
	unsigned int space = calculate_space(sizeof(StackMemoryNode), sizeof(size_t));
	if (self->offset + padding + size > self->size)
	{
		printf("stack: alloc failed, insufficient memory\n");
		return NULL;
	}
	address += padding;
	self->offset += padding + size;
	if (self->offset > self->peak)
		self->peak = self->offset;

	StackMemoryNode *node = (StackMemoryNode *)(address - space);

	if (self->head == NULL)
		byte7a(&node->data, 0, padding);
	else
		byte7a(&node->data, (size_t)self->head - start, padding);

	self->head = node;

	return (void *)address;
}

unsigned char stack_free(StackMemory *self, void **p)
{
	if (self == NULL)
	{
		printf("stack: free failed, invalid instance\n");
		return 0;
	}
	if (p == NULL || (*p) == NULL)
	{
		printf("stack: free failed, invalid pointer\n");
		return 0;
	}
	size_t start = (size_t)self - self->padding;
	size_t address = (size_t)(*p);
	size_t end = start + self->size;
	if (!(address >= start || address < end))
	{
		printf("stack: free failed, out of boundary\n");
		return 0;
	}
	if (address >= start + self->offset)
	{
		printf("stack: free failed, invalid order of free\n");
		return 0;
	}
	unsigned int space = calculate_space(sizeof(StackMemoryNode), sizeof(size_t));
	StackMemoryNode *node = (StackMemoryNode *)(address - space);
	if (node != self->head)
	{
		printf("stack: free failed, you must free the stack in order\n");
		return 0;
	}

	size_t offset;
	unsigned char nodePad;
	byte7d(node->data, &offset, &nodePad);
	self->offset = (address - start) - nodePad;
	StackMemoryNode *head = (StackMemoryNode *)self->head;

	byte7d(head->data, &offset, &nodePad);
	if (offset == 0)
		self->head = NULL;
	else
		self->head = (StackMemoryNode *)(start + offset);
	*p = NULL;
	return 1;
}

void stack_reset(StackMemory *self)
{
	if (self == NULL)
	{
		printf("stack: reset failed, invalid instance\n");
		return;
	}
	StackMemory *instance = self;
	unsigned int space = calculate_space(sizeof(StackMemory), sizeof(size_t));
	instance->offset = instance->padding + space;
	instance->peak = instance->offset;
	instance->head = NULL;
}

void stack_destroy(StackMemory **self)
{
	if (self == NULL || *self == NULL)
	{
		printf("stack: destroy failed, invalid instance\n");
		return;
	}
	StackMemory *instance = *self;
	size_t op = (size_t)instance - instance->padding;
	free((void *)(op));
	*self = NULL;
}

StackMemory *make_stack(size_t size)
{
	void *m = malloc(size);
	if (m == NULL)
	{
		printf("stack: make failed, system can't provide free memory\n");
		exit(EXIT_FAILURE);
		return NULL;
	}
	size_t address = (size_t)m;
	unsigned int space = calculate_space(sizeof(StackMemory), sizeof(size_t));
	unsigned int padding = calculate_padding(address, sizeof(size_t));
	StackMemory *instance = (StackMemory *)(address + padding);
	instance->head = NULL;
	instance->size = size;
	instance->offset = padding + space;
	instance->peak = instance->offset;
	instance->padding = padding;
	return instance;
}
