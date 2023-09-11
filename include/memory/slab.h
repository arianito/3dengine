#pragma once

#include <stddef.h>

typedef struct
{
	void *next;
	char used;
} SlabObject;

typedef struct
{
	void *next;
	unsigned int size;
	unsigned int padding;
} SlabPage;

typedef struct
{
	SlabObject *objects;
	SlabPage *pages;
	unsigned int padding;
	unsigned int slabSize;
	unsigned int objectSize;
	unsigned int capacity;
	void* (*allocator)(size_t);
} SlabMemory;

SlabMemory *make_slab(unsigned int slabSize, unsigned short objectSize);
void slab_destroy(SlabMemory **self);

void *slab_alloc(SlabMemory *self);
char slab_free(SlabMemory *self, void **ptr);