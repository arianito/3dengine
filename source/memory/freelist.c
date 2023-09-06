#include "memory/freelist.h"

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "memory/utils.h"

#define NODE_SPACE() (calculate_space(sizeof(FreeListMemory), sizeof(size_t)))

#define NODE_LOWER(node) ((size_t)(node) + NODE_SPACE() - (node)->padding)
#define NODE_HIGHER(node) ((size_t)(node) + NODE_SPACE())

void freelist_first(FreeListMemory *self, size_t size, unsigned int alignment, unsigned int *outPadding, FreeListMemory **outPrevNode, FreeListMemory **outNode)
{
	const unsigned int space = calculate_space(sizeof(FreeListMemory), sizeof(size_t));
	FreeListMemory
		*node = self->next,
		*prev = NULL;
	unsigned int padding;
	while (node != NULL)
	{
		padding = calculate_alignment(NODE_LOWER(node), sizeof(FreeListMemory), alignment);
		if (node->size >= size + padding + space)
			break;

		prev = node;
		node = node->next;
	}
	*outPrevNode = prev;
	*outNode = node;
	*outPadding = padding;
}

void freelist_insert(FreeListMemory *self, FreeListMemory *prevNode, FreeListMemory *newNode)
{
	if (prevNode == NULL)
	{ // first node
		if (self->next != NULL)
		{
			newNode->next = self->next;
		}
		else
		{
			newNode->next = NULL;
		}
		self->next = newNode;
	}
	else
	{
		if (prevNode->next == NULL)
		{ // end node
			prevNode->next = newNode;
			newNode->next = NULL;
		}
		else
		{ // middle node
			newNode->next = prevNode->next;
			prevNode->next = newNode;
		}
	}
}

void freelist_remove(FreeListMemory *self, FreeListMemory *prevNode, FreeListMemory *node)
{
	if (prevNode == NULL)
	{ // first node
		if (node->next == NULL)
		{
			self->next = NULL;
		}
		else
		{
			self->next = node->next;
		}
	}
	else
	{
		prevNode->next = node->next;
	}
}

void freelist_joinnext(FreeListMemory *self, FreeListMemory *previousNode, FreeListMemory *freeNode)
{
	FreeListMemory *next = (FreeListMemory *)freeNode->next;
	if (next != NULL && NODE_LOWER(freeNode) + freeNode->size == NODE_LOWER(next))
	{
		freeNode->size += next->size;
		freelist_remove(self, freeNode, freeNode->next);
	}

	if (previousNode != NULL && NODE_LOWER(previousNode) + previousNode->size == NODE_LOWER(freeNode))
	{
		previousNode->size += freeNode->size;
		freelist_remove(self, previousNode, freeNode);
	}
}

void *freelist_alloc(FreeListMemory *self, size_t size, unsigned int alignment)
{
	if ((alignment & (alignment - 1)) != 0)
	{
		printf("freelist: alloc failed, invalid alignment\n");
		return NULL;
	}
	if (self == NULL)
	{
		printf("freelist: alloc failed, invalid instance\n");
		return NULL;
	}
	unsigned int padding;
	FreeListMemory *prevNode;
	FreeListMemory *node;
	freelist_first(self, size, alignment, &padding, &prevNode, &node);

	if (node == NULL)
	{
		printf("freelist: alloc failed, insufficient memory\n");
		return NULL;
	}

	unsigned int space = calculate_space(sizeof(FreeListMemory), sizeof(size_t));

	size_t requiredSpace = size + padding;
	size_t remainingSpace = node->size - requiredSpace;

	size_t addr = NODE_LOWER(node);
	void *tmp = node->next;
	node = (FreeListMemory *)(addr + padding - space);
	node->padding = padding;
	node->next = tmp;

	node->size = requiredSpace;

	if (remainingSpace > 0)
	{
		FreeListMemory *newNode = (FreeListMemory *)(addr + requiredSpace);
		newNode->padding = space;
		newNode->size = remainingSpace;
		freelist_insert(self, node, newNode);
	}
	freelist_remove(self, prevNode, node);

	return (void *)NODE_HIGHER(node);
}

unsigned char freelist_free(FreeListMemory *self, void **ptr)
{

	if (self == NULL)
	{
		printf("freelist: free failed, invalid instance\n");
		return 0;
	}
	if (ptr == NULL || (*ptr) == NULL)
	{
		printf("freelist: free failed, invalid pointer\n");
		return 0;
	}

	size_t start = NODE_LOWER(self);
	size_t address = (size_t)(*ptr);
	size_t end = start + self->size;
	if (!(address >= start && address < end))
	{
		printf("freelist: free failed, out of boundary\n");
		return 0;
	}

	unsigned int space = calculate_space(sizeof(FreeListMemory), sizeof(size_t));

	FreeListMemory *freeedNode = (FreeListMemory *)((size_t)(*ptr) - space);
	freeedNode->next = NULL;

	FreeListMemory *prevNode = NULL;
	FreeListMemory *node = self->next;

	while (node != NULL)
	{
		if (*ptr < (void *)node)
		{
			freelist_insert(self, prevNode, freeedNode);
			break;
		}
		prevNode = node;
		node = node->next;
	}

	freelist_joinnext(self, prevNode, freeedNode);

	if (prevNode == NULL && freeedNode == self->next)
	{
		size_t lower = NODE_LOWER(freeedNode);
		size_t higher = NODE_HIGHER(freeedNode);
		void *tmp = freeedNode->next;
		size_t size = freeedNode->size;

		node = (FreeListMemory *)(lower);
		node->padding = space;
		node->size = size;
		node->next = tmp;
		self->next = node;
	}

	*ptr = NULL;
	return 1;
}

void freelist_destroy(FreeListMemory **self)
{
	if (self == NULL || *self == NULL)
	{
		printf("freelist: destroy failed, invalid instance\n");
		return;
	}
	free((void *)NODE_LOWER(*self));
	*self = NULL;
}

void freelist_reset(FreeListMemory *self)
{
	const unsigned int space = calculate_space(sizeof(FreeListMemory), sizeof(size_t));
	size_t start = NODE_HIGHER(self);
	unsigned int padding = calculate_alignment(start, sizeof(FreeListMemory), sizeof(size_t));

	FreeListMemory *node = (FreeListMemory *)start;

	self->next = NULL;
	node->padding = padding;
	node->size = self->size - (self->padding);
	node->next = NULL;

	freelist_insert(self, NULL, node);
}

FreeListMemory *freelist_create(void *m, size_t size)
{
	size_t address = (size_t)m;
	const unsigned int space = calculate_space(sizeof(FreeListMemory), sizeof(size_t));
	const unsigned int padding = calculate_alignment(address, sizeof(FreeListMemory), sizeof(size_t));

	FreeListMemory *self = (FreeListMemory *)(address + padding - space);
	self->size = size;
	self->padding = padding;

	freelist_reset(self);
	return self;
}

FreeListMemory *make_freelist(size_t size)
{
	void *m = malloc(size);
	if (m == NULL)
	{
		printf("freelist: make failed, system can't provide free memory\n");
		exit(EXIT_FAILURE);
		return NULL;
	}
	return freelist_create(m, size);
}
