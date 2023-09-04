

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "memory/freelist.h"
#include "memory/utils.h"

typedef struct
{
	void *next;
	void *prev;
	size_t size;
	size_t padding;
} FreeListMemoryNode;

void freelist_insert(FreeListMemory *self, FreeListMemoryNode *node, FreeListMemoryNode *newNode)
{
	if (node == NULL)
	{
		newNode->next = self->head;
		newNode->prev = NULL;

		if (self->head != NULL)
			((FreeListMemoryNode *)(self->head))->prev = newNode;

		self->head = newNode;
		return;
	}

	if (node->next == NULL)
	{
		node->next = newNode;
		newNode->prev = node;
		newNode->next = NULL;
		return;
	}

	newNode->next = node->next;
	node->next = newNode;
	newNode->prev = node;
}

void freelist_remove(FreeListMemory *self, FreeListMemoryNode *prevNode, FreeListMemoryNode *node)
{
	if (prevNode == NULL)
	{
		self->head = node->next;
		if (self->head != NULL)
			((FreeListMemoryNode *)(self->head))->prev = NULL;
		return;
	}

	prevNode->next = node->next;
	if (node->next != NULL)
		((FreeListMemoryNode *)(node->next))->prev = prevNode;
}