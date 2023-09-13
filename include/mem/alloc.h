#pragma once
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

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "mem/memory.h"
#include "mem/arena.h"
#include "mem/stack.h"
#include "mem/freelist.h"

typedef struct
{
	size_t global;
	size_t stack;
	size_t freelist;
} MemoryMetadata;

typedef struct
{
	MemoryMetadata metadata;
	ArenaMemory *global;
	StackMemory *stack;
	FreeListMemory *freelist;
} MemoryLayout;

extern MemoryLayout *alloc;

void alloc_create(MemoryMetadata meta);
void alloc_terminate();
void alloc_debug();

#define alloc_global(Type, size) ((Type *)arena_alloc(alloc->global, size, sizeof(size_t)))
#define alloc_stack(Type, size) ((Type *)stack_alloc(alloc->stack, size, sizeof(size_t)))
#define alloc_free(p) (stack_free(alloc->stack, p))