#include "memory/freelist.h"

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "memory/utils.h"

void freelist_first(FreeListMemory *self, size_t size, FreeListMemoryNode **outPrevNode, FreeListMemoryNode **outNode)
{
	FreeListMemoryNode
		*node = self->head,
		*prev = NULL;
	while (node != NULL)
	{
		size_t nodeSize;
		unsigned short padding;
		byte6d(node->data, &nodeSize, &padding);
		if (nodeSize >= size + padding)
			break;

		prev = node;
		node = node->next;
	}
	*outPrevNode = prev;
	*outNode = node;
}

void freelist_insert(FreeListMemory *self, FreeListMemoryNode *prevNode, FreeListMemoryNode *newNode)
{
	if (prevNode == NULL)
	{ // first node
		if (self->head != NULL)
		{
			newNode->next = self->head;
		}
		else
		{
			newNode->next = NULL;
		}
		self->head = newNode;
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

void freelist_remove(FreeListMemory *self, FreeListMemoryNode *prevNode, FreeListMemoryNode *node)
{
	if (prevNode == NULL)
	{ // first node
		if (node->next == NULL)
		{
			self->head = NULL;
		}
		else
		{
			self->head = node->next;
		}
	}
	else
	{
		prevNode->next = node->next;
	}
}

void freelist_joinnext(FreeListMemory *self, FreeListMemoryNode *prevNode, FreeListMemoryNode *freeNode)
{
	unsigned int space = calculate_space(sizeof(FreeListMemoryNode), sizeof(size_t));

	if (freeNode->next != NULL)
	{
		size_t nodeSize;
		unsigned short nodePadding;
		byte6d(freeNode->data, &nodeSize, &nodePadding);

		size_t nodeNextSize;
		unsigned short nodeNextPadding;
		byte6d(((FreeListMemoryNode *)freeNode->next)->data, &nodeNextSize, &nodeNextPadding);

		size_t start = (size_t)freeNode + space - nodePadding;
		size_t end = (size_t)freeNode->next + space - nodeNextPadding;
		if (start + nodeSize == end)
		{
			byte6a6(&freeNode->data, nodeSize + nodeNextSize);
			freelist_remove(self, freeNode, freeNode->next);
		}
	}

	if (prevNode != NULL)
	{
		size_t nodeSize;
		unsigned short nodePadding;
		byte6d(prevNode->data, &nodeSize, &nodePadding);

		size_t nodeNextSize;
		unsigned short nodeNextPadding;
		byte6d(freeNode->data, &nodeNextSize, &nodeNextPadding);

		size_t start = (size_t)prevNode + space - nodePadding;
		size_t end = (size_t)freeNode + space - nodeNextPadding;
		if (start + nodeSize == end)
		{
			byte6a6(&prevNode->data, nodeSize + nodeNextSize);
			freelist_remove(self, prevNode, freeNode);
		}
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

	FreeListMemoryNode *prevNode;
	FreeListMemoryNode *node;
	freelist_first(self, size, &prevNode, &node);

	if (node == NULL)
	{
		printf("freelist: alloc failed, insufficient memory\n");
		return NULL;
	}

	unsigned int space = calculate_space(sizeof(FreeListMemoryNode), sizeof(size_t));

	size_t nodeSize;
	unsigned short nodePadding;
	byte6d(node->data, &nodeSize, &nodePadding);
	size_t requiredSpace = size + nodePadding;
	size_t remainingSpace = nodeSize - requiredSpace;
	size_t blockStart = (size_t)node + space - nodePadding;

	if (remainingSpace > 0)
	{
		size_t newAddress = blockStart + requiredSpace;
		unsigned int padding = calculate_alignment(newAddress, sizeof(FreeListMemoryNode), sizeof(size_t));

		FreeListMemoryNode *newNode = (FreeListMemoryNode *)(newAddress + padding - space);
		byte6a(&newNode->data, remainingSpace, padding);
		freelist_insert(self, node, newNode);
	}
	freelist_remove(self, prevNode, node);
	byte6a6(&node->data, requiredSpace);

	return (void *)((size_t)node + space);
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

	size_t start = (size_t)self - self->padding;
	size_t address = (size_t)(*ptr);
	size_t end = start + self->size;
	if (!(address >= start && address < end))
	{
		printf("freelist: free failed, out of boundary\n");
		return 0;
	}

	unsigned int space = calculate_space(sizeof(FreeListMemoryNode), sizeof(size_t));

	FreeListMemoryNode *freeedNode = (FreeListMemoryNode *)((size_t)(*ptr) - space);
	freeedNode->next = NULL;

	FreeListMemoryNode *prevNode = NULL;
	FreeListMemoryNode *node = self->head;

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
	size_t op = (size_t)(*self) - (*self)->padding;
	free((void *)(op));
	*self = NULL;
}

void freelist_reset(FreeListMemory *self)
{
	unsigned int selfSpace = calculate_space(sizeof(FreeListMemory), sizeof(size_t));
	size_t start = (size_t)self - self->padding;
	size_t end = start + self->size;

	size_t blockStart = start + (self->padding + selfSpace);

	unsigned int padding = calculate_alignment(blockStart, sizeof(FreeListMemoryNode), sizeof(size_t));
	unsigned int space = calculate_space(sizeof(FreeListMemoryNode), sizeof(size_t));

	FreeListMemoryNode *node = (FreeListMemoryNode *)(blockStart + padding - space);
	node->next = NULL;
	freelist_insert(self, NULL, node);
	byte6a(&node->data, end - blockStart, padding);
}

FreeListMemory *freelist_create(void *m, size_t size)
{
	size_t address = (size_t)m;
	unsigned int padding = calculate_padding(address, sizeof(size_t));
	FreeListMemory *self = (FreeListMemory *)(address + padding);
	self->size = size;
	self->padding = padding;
	self->head = NULL;

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
