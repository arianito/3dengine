#pragma once

#include "stddef.h"

typedef struct
{
    void *pool;
} MemoryFootprint;

typedef struct
{
    size_t total_memory;
} MemoryDef;

MemoryFootprint *footprint;

void footprint_create(MemoryDef *def);
void footprint_terminate();