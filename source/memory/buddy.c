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

#include "memory/buddy.h"

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "memory/utils.h"

BuddyMemory *buddy_create(void *m, unsigned char level)
{
	size_t start = (size_t)m;
	const unsigned char padding = MEMORY_ALIGNMENT(start, 1, sizeof(size_t));
	unsigned int size = 1 << level;

	((char*)(start + padding - 1))[0] = padding;
	
	BuddyMemory *self = (BuddyMemory *)(start + padding);
	self->next = NULL;
	self->order = level;
	self->status = 0;

	return self;
}

BuddyMemory *make_buddy(unsigned char level)
{
	unsigned int size = 1 << level;
	size += MEMORY_SPACE_STD(BuddyMemory) + sizeof(size_t);
	void *m = malloc(size);
	if (m == NULL)
	{
		printf("buddy: make failed, system can't provide free memory\n");
		exit(EXIT_FAILURE);
		return NULL;
	}
	memset(m, 0, size);
	return buddy_create(m, level);
}

void *buddy_alloc(BuddyMemory *self, size_t size)
{
	if (!ISPOW2(size))
	{
		printf("buddy: alloc failed, invalid alignment\n");
		return NULL;
	}
	if (self == NULL)
	{
		printf("buddy: alloc failed, invalid instance\n");
		return NULL;
	}
	return NULL;
}
unsigned char buddy_free(BuddyMemory *self, void **ptr)
{
	if (self == NULL)
	{
		printf("buddy: free failed, invalid instance\n");
		return 0;
	}
	if (ptr == NULL || (*ptr) == NULL)
	{
		printf("buddy: free failed, invalid pointer\n");
		return 0;
	}
	return 0;
}