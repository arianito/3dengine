#include "footprint.h"
#include "mem.h"
#include "malloc.h"

void footprint_create(MemoryDef *def)
{
    footprint = (MemoryFootprint *)malloc(sizeof(MemoryFootprint));
    footprint->pool = NULL;
}

void footprint_terminate()
{
    free(footprint);
}