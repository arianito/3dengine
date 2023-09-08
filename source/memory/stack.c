/******************************************************************************
 *                                                                            *
 *  Copyright (c) 2023 Aryan Alikhani                                      *
 *  GitHub: github.com/arianito                                               *
 *  Email: alikhaniaryan@gmail.com                                            *
 *                                                                            *
 *  Permission is hereby granted, free of charge, to any person obtaining a   *
 *  copy of this software and associated documentation files (the "Software"),*
 *  to deal in the Software without restriction, including without limitation *
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,  *
 *  and/or sell copies of the Software, and to permit persons to whom the      *
 *  Software is furnished to do so, subject to the following conditions:       *
 *                                                                            *
 *  The above copyright notice and this permission notice shall be included   *
 *  in all copies or substantial portions of the Software.                    *
 *                                                                            *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS   *
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF                *
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN *
 *  NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   *
 *  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR     *
 *  OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  *
 *  USE OR OTHER DEALINGS IN THE SOFTWARE.                                   *
 *                                                                            *
 *****************************************************************************/
#include "memory/stack.h"

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

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
	const unsigned int padding = MEMORY_ALIGNMENT(address, sizeof(StackMemoryNode), alignment);
	const unsigned int space = MEMORY_SPACE(sizeof(StackMemoryNode), sizeof(size_t));
	if (self->offset + padding + size > self->size)
	{
		printf("stack: alloc failed, insufficient memory\n");
		return NULL;
	}
	self->offset += padding + size;
	if (self->offset > self->peak)
		self->peak = self->offset;

	StackMemoryNode *node = (StackMemoryNode *)(address + padding - space);

	if (self->head == NULL)
		node->data = BYTE71(0, padding);
	else
		node->data = BYTE71((size_t)self->head - start, padding);

	self->head = node;

	return (void *)(address + padding);
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
	if (!(address >= start && address < end))
	{
		printf("stack: free failed, out of boundary\n");
		return 0;
	}
	if (address >= start + self->offset)
	{
		printf("stack: free failed, invalid order of free\n");
		return 0;
	}
	const unsigned int space = MEMORY_SPACE_STD(StackMemoryNode);
	StackMemoryNode *node = (StackMemoryNode *)(address - space);
	if (node != self->head)
	{
		printf("stack: free failed, you must free the stack in order\n");
		return 0;
	}

	size_t offset = BYTE71_GET_7(node->data);
	unsigned char padding = BYTE71_GET_1(node->data);

	self->offset = (address - padding) - start;
	StackMemoryNode *head = (StackMemoryNode *)self->head;

	offset = BYTE71_GET_7(head->data);

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
	const unsigned int space = MEMORY_SPACE_STD(StackMemory);
	self->offset = self->padding + space;
	self->peak = self->offset;
	self->head = NULL;
}

void stack_destroy(StackMemory **self)
{
	if (self == NULL || *self == NULL)
	{
		printf("stack: destroy failed, invalid instance\n");
		return;
	}
	size_t op = (size_t)(*self) - (*self)->padding;
	free((void *)(op));
	*self = NULL;
}

StackMemory *stack_create(void *m, size_t size)
{
	size_t address = (size_t)m;
	const unsigned int space = MEMORY_SPACE_STD(StackMemory);
	const unsigned int padding = MEMORY_PADDING_STD(address);
	StackMemory *self = (StackMemory *)(address + padding);
	self->head = NULL;
	self->size = size;
	self->offset = padding + space;
	self->peak = self->offset;
	self->padding = padding;
	return self;
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
	return stack_create(m, size);
}

StackMemory *make_stack_exact(size_t size)
{
	size += MEMORY_SPACE_STD(StackMemory) + sizeof(size_t);
	return make_stack(size);
}
