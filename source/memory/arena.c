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
#include "memory/arena.h"

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "memory/utils.h"

void *arena_alloc(ArenaMemory *self, size_t size, unsigned int alignment)
{
	if ((alignment & (alignment - 1)) != 0)
	{
		printf("arena: alloc failed, invalid alignment\n");
		return NULL;
	}
	if (self == NULL)
	{
		printf("arena: alloc failed, invalid instance\n");
		return NULL;
	}
	size_t address = ((size_t)self - self->padding) + self->offset;
	int padding = MEMORY_PADDING(address, alignment);
	if (self->offset + size + padding > self->size)
	{
		printf("arena: alloc failed, insufficient memory\n");
		return NULL;
	}
	address += padding;
	self->offset += size + padding;
	return (void *)(address);
}

void arena_reset(ArenaMemory *self)
{
	if (self == NULL)
	{
		printf("arena: reset failed, invalid instance\n");
		return;
	}
	const unsigned int space = MEMORY_SPACE_STD(ArenaMemory);
	self->offset = self->padding + space;
}

void arena_destroy(ArenaMemory **self)
{
	if (self == NULL || *self == NULL)
	{
		printf("arena: destroy failed, invalid instance\n");
		return;
	}
	size_t op = (size_t)(*self) - (*self)->padding;
	free((void *)(op));
	*self = NULL;
}

ArenaMemory *arena_create(void *m, size_t size)
{
	size_t address = (size_t)m;
	const unsigned int space = MEMORY_SPACE_STD(ArenaMemory);
	const unsigned int padding = MEMORY_PADDING_STD(address);
	ArenaMemory *self = (ArenaMemory *)(address + padding);
	self->size = size;
	self->offset = padding + space;
	self->padding = padding;
	return self;
}

ArenaMemory *make_arena(size_t size)
{
	void *m = malloc(size);
	if (m == NULL)
	{
		printf("arena: make failed, system can't provide free memory\n");
		exit(EXIT_FAILURE);
		return NULL;
	}
	return arena_create(m, size);
}

ArenaMemory *make_arena_exact(size_t size)
{
	size += MEMORY_SPACE_STD(ArenaMemory) + sizeof(size_t);
	return make_arena(size);
}
