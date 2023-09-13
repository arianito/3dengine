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
#include "mem/alloc.h"

#include <malloc.h>
#include "mem/std.h"

MemoryLayout *alloc = NULL;

void alloc_create(MemoryMetadata meta) {
    alloc = std_alloc(sizeof(MemoryLayout), sizeof(size_t));
    alloc->metadata = meta;
    alloc->global = make_arena(meta.global);
    alloc->stack = stack_create(arena_alloc(alloc->global, meta.stack, sizeof(size_t)), meta.stack);
    alloc->freelist = freelist_create(arena_alloc(alloc->global, meta.freelist, sizeof(size_t)), meta.freelist);
}

void alloc_terminate() {
    arena_destroy(&alloc->global);
    std_free(&alloc);
}

void alloc_debug() {
    printf("global: %zu/%zu \n", alloc->global->offset, alloc->global->size);
    printf("stack: %zu/%zu \n", alloc->stack->offset, alloc->stack->size);
    printf("freelist: %zu/%zu \n", freelist_capacity(alloc->freelist), alloc->freelist->size);
}